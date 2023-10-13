#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <vector>

#include "section.h"
#include "schedule_time.h"

AsyncWebServer server { 80 };

std::vector<Section> section_list;

String get_content_type(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    // Add more content types if needed

    return "text/plain";
}

void handle_static_resource(AsyncWebServerRequest* request) {
    // Get the requested file path
    String path = request->url();

    request->send(SPIFFS, path, get_content_type(path));
}

void handle_root_page(AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html");
}

void handle_form_submit(AsyncWebServerRequest* request) {
    Section theSection;

    // -- this whole thing should be abstracted --

    if (!request->hasParam("id", true)) {
        // somehow report error
        Serial.println("id error");
        request->redirect("/"); // god i miss "defer"
        return;
    }

    theSection.set_id(request->getParam("id", true)->value().c_str());

    if (!request->hasParam("name", true)) {
        // somehow report error
        Serial.println("name error");
        request->redirect("/");
        return;
    }

    theSection.set_name(request->getParam("name", true)->value().c_str());

    for (std::string day: days) {
        String A_str { day.c_str() };
        if (request->hasParam(A_str, true)) {
            theSection.set_day(Section::str_to_day(day), true);
        }
    }

    if (!request->hasParam("start_time", true)) {
        Serial.println("start time error");
        request->redirect("/");
        // error
        return;
    }

    theSection.set_time(Side::START, request->getParam("start_time", true)->value().c_str());

    if (!request->hasParam("end_time", true)) {
        // error
        Serial.println("end time error");
        request->redirect("/");
        return;
    }

    theSection.set_time(Side::END, request->getParam("end_time", true)->value().c_str());

    bool del {request->hasParam("delete", true)};

    // manually search -_-
    bool exists { false };
    for (auto it{section_list.begin()}; it != section_list.end(); it++) {
        if (it->get_id() == theSection.get_id()) {
            exists = true;
            if (del) {
                section_list.erase(it);
            } else {
                *it = theSection;
            }
            break;
        }
    }

    if (!exists && !del) {
        section_list.push_back(theSection);
    }

    // Redirect back to the root page after form submission
    request->redirect("/");
}

void handle_schedule(AsyncWebServerRequest* request) {
    StaticJsonDocument<5000> theDoc;

    JsonArray theSchedule = theDoc.createNestedArray("schedule");

    for (auto it { section_list.begin() }; it != section_list.end(); it++) {
        JsonObject theObj = theSchedule.createNestedObject();

        it->serialize(theObj);
    }

    String theResponse;
    serializeJson(theDoc, theResponse);

    request->send(200, "application/json", theResponse);
}

void handle_time(AsyncWebServerRequest* request) {
    if (!request->hasParam("year", true)) {
        // error
        request->redirect("/");
        return;
    }

    int year { request->getParam("year", true)->value().toInt() };

    if (!request->hasParam("month", true)) {
        // error
        request->redirect("/");
        return;
    }

    int month { request->getParam("month", true)->value().toInt() };

    if (!request->hasParam("day", true)) {
        // error
        request->redirect("/");
        return;
    }

    int day { request->getParam("day", true)->value().toInt() };

    if (!request->hasParam("hour", true)) {
        // error
        request->redirect("/");
        return;
    }

    int hour { request->getParam("hour", true)->value().toInt() };

    if (!request->hasParam("minute", true)) {
        // error
        request->redirect("/");
        return;
    }

    int minute { request->getParam("minute", true)->value().toInt() };

    rtc.setTime(0, minute, hour, day, month, year);

    request->redirect("/");
}

void setup_web_server() {
    Serial.begin(9600);
    SPIFFS.begin();

    server.on("/", HTTP_GET, handle_root_page);
    server.on("/static", HTTP_GET, handle_static_resource);
    server.on("/submit", HTTP_POST, handle_form_submit);
    server.on("/schedule", HTTP_GET, handle_schedule);
    server.on("/time", HTTP_POST, handle_time);
    server.begin();
}