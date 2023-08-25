#include <WiFi.h>

// WiFi defs
#define SSID "EnVision-Local"
#define PSWD "thinkmakebreak"

String setup_WiFi() {
    WiFi.begin(SSID, PSWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    return WiFi.localIP().toString();
}