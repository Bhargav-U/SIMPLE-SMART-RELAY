#ifndef INDICATOR_h
#define INDICATOR_h

#include <Adafruit_NeoPixel.h>
#include <CLOUD.h>

#define PIN_WS2812B 4
#define NUM_PIXELS 1
Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

bool SetupMode = false;

// CHECK FOR WIFI CONNECTION FIRST,THEN BASED ON STATUS SET THE COLOUR.
// IF WIFI IS CONNECTED BUT WEAK ,BASED ON IT DIM THE GREEN
// NO WIFI MEANS RED
// ANY ISSUES WITH SENDING THE DATA TO CLOUD MEANS PURPLE
void ShowStatus()
{
    IsWiFiConnected();
    ws2812b.clear();
    if (SetupMode)
    {
        ws2812b.setPixelColor(0, ws2812b.Color(0, 0, 255));
    }
    else
    {
        if (!WifiStatus)
        {
            ws2812b.setPixelColor(0, ws2812b.Color(255, 0, 0));
        }
        else
        {
            if (!CloudStatus)
            {
                ws2812b.setPixelColor(0, ws2812b.Color(255, 0, 255));
            }
            else
            {
                int signalStrength = WiFi.RSSI();
                Serial.println("Signal strength (RSSI): " + String(signalStrength));

                if (signalStrength > -50)
                {
                    ws2812b.setPixelColor(0, ws2812b.Color(0, 255, 0)); // Excellent signal
                }
                else if (signalStrength > -60)
                {
                    ws2812b.setPixelColor(0, ws2812b.Color(50, 255, 0)); // Good signal
                }
                else if (signalStrength > -70)
                {
                    ws2812b.setPixelColor(0, ws2812b.Color(100, 255, 0)); // Fair signal
                }
                else if (signalStrength > -80)
                {
                    ws2812b.setPixelColor(0, ws2812b.Color(150, 255, 0)); // Weak signal
                }
                else
                {
                    ws2812b.setPixelColor(0, ws2812b.Color(255, 255, 0)); // Very weak signal or no signal
                }
            }
        }
    }
    ws2812b.show();
}

void SetupIndicator()
{
    Serial.println("INDICATOR SETUP");
    ws2812b.begin();
    ShowStatus();
}
#endif