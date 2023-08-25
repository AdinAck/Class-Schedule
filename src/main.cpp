#include "schedule_wifi.h"
#include "schedule_server.h"
#include "schedule_display.h"

void setup() {
    setup_time();

    setup_display();
    wake_display();

    String ip = setup_WiFi();

    setup_web_server();

    show_page([&] {
        display.fillScreen(GxEPD_WHITE);
        draw_text(ip, &SF_Pro_Display_Regular32pt7b);
    });

    sleep_display();

    delay(MS_PER_SECOND * 10);
}

int previous = -2;

void loop() {
    int day = rtc.getDayofWeek();
    int hour = rtc.getHour(true);
    int minute = rtc.getMinute();

    wake_display();

    int current = -1;

    for (auto it { section_list.begin() }; it != section_list.end(); it++) {
        Time start { it->get_time(Side::START) };
        Time end { it->get_time(Side::END) };

        if (!it->get_day((Day) day)) {
            continue;
        }

        if (hour < start.hour || (hour == start.hour && minute < start.minute)) {
            continue;
        }

        if (hour > end.hour || (hour == end.hour && minute > end.minute)) {
            continue;
        }

        current = it - section_list.begin();
        if (current != previous) {
            show_current("IN SESSION", it->get_name().c_str(), end.to_string().c_str());
        }

        break;
    }

    if (current == -1 && current != previous) {
        show_current("VACANT", "", "");
    }

    sleep_display();

    previous = current;

    delay(MS_PER_SECOND * SECONDS_PER_MINUTE); // wait 1 minute
}