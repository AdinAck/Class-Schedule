#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include "fonts/SF-Pro-Display-Regular32pt7b.h"
#include "fonts/SF-Pro-Display-Bold64pt7b.h"
#include <Fonts/FreeSansBold18pt7b.h>

#include <functional>

#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_750_T7
#define MAX_DISPLAY_BUFFER_SIZE 65536ul // 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display { GxEPD2_DRIVER_CLASS(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25) };

SPIClass hspi { HSPI };

void setup_display() {
    display.init(115200, true, 2, false);
}

void wake_display() {
    hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
    display.epd2.selectSPI(hspi, SPISettings { 4000000, MSBFIRST, SPI_MODE0 });
    display.setRotation(0);
    display.setFullWindow();
}

void sleep_display() {
    display.hibernate();
}

void show_page(std::function<void()> page) {
    display.firstPage();

    do {
        page();
    } while (display.nextPage());
}

// enum Alignment {
//     LEFT,
//     RIGHT,
//     CENTER
// };

// enum Origin
// {
//     TOP_LEFT,
//     BOTTOM_LEFT,
//     CENTER
// };

void draw_text(String text, const GFXfont* font) {
    int16_t tbx, tby; uint16_t tbw, tbh;

    display.setFont(font);
    display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setCursor(x, y);
    display.setTextColor(GxEPD_BLACK);
    display.print(text);
}

const uint16_t padding { 16 };

void show_current(String status, String class_name, String end_time) {
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        int16_t tbx, tby; uint16_t tbw, tbh;
        uint16_t x, y;

        display.setFont(&SF_Pro_Display_Regular32pt7b);
        display.getTextBounds(status, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = 50;
        y = 50 + tbh;
        display.setCursor(x - 5, y - 2);
        display.fillRect(x - padding, y - tbh - padding, tbw + 2 * padding, tbh + 2 * padding, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
        display.print(status);

        display.setFont(&SF_Pro_Display_Bold64pt7b);
        display.getTextBounds(class_name, 0, 0, &tbx, &tby, &tbw, &tbh);
        x = 100;
        y = (display.height() + tbh) / 2;
        display.setCursor(x - 5, y - 2);
        display.setTextColor(GxEPD_BLACK);
        display.print(class_name);

        if (end_time != "") {
            display.setFont(&FreeSansBold18pt7b);
            display.getTextBounds(end_time, 0, 0, &tbx, &tby, &tbw, &tbh);
            x = 100;
            y = 320 + tbh;
            display.setCursor(x, y);
            display.setTextColor(GxEPD_BLACK);
            display.print("Until " + end_time);
        }

    } while (display.nextPage());
}