#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>

enum Side {
    START,
    END
};

enum Day {
    M = 1,
    Tu,
    W,
    Thu,
    F,
    Sa,
    Su = 0
};

const std::string days[] = { "M", "Tu", "W", "Thu", "F", "Sa", "Su" };

struct Time {
    uint8_t hour, minute;

    Time() : hour(0), minute(0) { }
    Time(uint8_t anHour, uint8_t aMinute) : hour(anHour), minute(aMinute) { }

    std::string to_string() {
        // sensitive little shit
        return (hour < 10 ? "0" : "") + std::to_string(hour) + ":" + (minute < 10 ? "0" : "") + std::to_string(minute);
    }
};

class Section {
public:
    std::string get_name() const {
        return name;
    }

    bool get_day(Day aDay) const {
        switch (aDay) {
        case Day::M:
            return m;
        case Day::Tu:
            return tu;
        case Day::W:
            return w;
        case Day::Thu:
            return thu;
        case Day::F:
            return f;
        case Day::Sa:
            return sa;
        case Day::Su:
            return su;
        };

        return false;
    }

    Time get_time(Side aSide) const {
        switch (aSide) {
        case Side::START:
            return start_time;
        case Side::END:
            return end_time;
        };

        return Time();
    }

    void set_name(std::string aName) {
        name = aName;
    }

    bool set_day(Day aDay, bool aValue) {
        switch (aDay) {
        case Day::M:
            m = aValue;
            return true;
        case Day::Tu:
            tu = aValue;
            return true;
        case Day::W:
            w = aValue;
            return true;
        case Day::Thu:
            thu = aValue;
            return true;
        case Day::F:
            f = aValue;
            return true;
        case Day::Sa:
            sa = aValue;
            return true;
        case Day::Su:
            su = aValue;
            return true;
        };

        return false;
    }

    bool set_time(Side aSide, std::string aStrTime) {
        uint8_t hour, minute;

        // embedded rust high level structures would have worked...
        // C++ couldn't handle it

        size_t pivot;
        bool valid { false };

        for (size_t i{0}; i < aStrTime.size(); i++) {
            if (aStrTime.at(i) == ':') {
                pivot = i;
                valid = true;
                break;
            }
        }

        hour = std::stoi(aStrTime.substr(0, pivot));
        minute = std::stoi(aStrTime.substr(pivot + 1));

        if (!valid) {
            // invalid time string
            return false;
        }

        switch (aSide) {
        case Side::START:
            start_time = Time { hour, minute };
            break;
        case Side::END:
            end_time = Time { hour, minute };
            break;
        };

        return true;
    }

    // i wish i knew how to put this in Day
    static Day str_to_day(std::string aStr) {
        if (aStr == "M") {
            return Day::M;
        } else if (aStr == "Tu") {
            return Day::Tu;
        } else if (aStr == "W") {
            return Day::W;
        } else if (aStr == "Thu") {
            return Day::Thu;
        } else if (aStr == "F") {
            return Day::F;
        } else if (aStr == "Sa") {
            return Day::Sa;
        } else if (aStr == "Su") {
            return Day::Su;
        } else {
            return Day::M; // so dumb
        }
    }

    void serialize(JsonObject& anObj) const {
        anObj["name"] = get_name();

        for (std::string day: days) {
            anObj[day] = get_day(Section::str_to_day(day));
        }

        anObj["start_time"] = get_time(Side::START).to_string();
        anObj["end_time"] = get_time(Side::END).to_string();
    }

protected:
    std::string name;
    bool m = false, tu = false, w = false, thu = false, f = false, sa = false, su = false;
    Time start_time, end_time;
};