#ifndef CLOUD_h
#define CLOUD_h

#include <WiFi.h>
#include <HTTPClient.h>

const char *host = "us-east-1-1.aws.cloud2.influxdata.com";
const char *url = "/api/v2/query";
const char *token = "Yh8K0ag18ahXawH7e-Gkb_43bW95q4u90eK7DbgC0aYOxWMrX2Aa0ApwN15QvyKR2Qq8aKTDacEa8D9VG8hBQg==";
const char *bucket = "MINI_RELAY";
const char *WriteUrl = "/api/v2/write?org=5086e719fd5e3b56&bucket=MINI_RELAY";

const char *ssid = "dumbphone";
const char *password = "mamamia123";

String DeviceMac = "";

// Define the structure for storing credentials
struct OnBoardCredentials
{
  String SSID;
  String PASSWORD;
};

// Global variable to hold the credentials
OnBoardCredentials Credentials;

bool OnBoardCredentialsFound = false;

bool WifiStatus = false;
bool CloudStatus = false;

bool RelayState = false;

bool CurrentCloudRelayState = false;
bool LastCloudRelayState = false;

HTTPClient http;

// WIFI STATUS CHECKER
void IsWiFiConnected()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WifiStatus = true;
    }
    else
    {
        WifiStatus = false;
    }
}

// SIMPLE WIFI CONNECTION FUNCTION
void ConnectToWifi()
{
    
    WiFi.begin(Credentials.SSID, Credentials.PASSWORD);
    unsigned long startTime = millis();
    while (millis() - startTime < 3000)
    {
      delay(100);
      Serial.print(".");
      if(WifiStatus){
        break;
      }
    }


}





// GET THE RELAY STATE FROM THE CLOUD AND SET IT IN THE CURRENT CLOUD RELAY STATE
void GetRelayStateFromCloud()
{
    if (WifiStatus)
    {
        String fullUrl = String("https://") + host + url;

        http.begin(fullUrl);
        http.addHeader("Authorization", String("Token ") + token);
        http.addHeader("Content-Type", "application/vnd.flux");
        http.addHeader("Accept", "application/json");

        String macAddress = WiFi.macAddress();
        String query = "from(bucket: \"" + String(bucket) + "\") |> range(start: -5m) |> filter(fn: (r) => r[\"_measurement\"] == \"MINI_RELAY\") |> filter(fn: (r) => r[\"DeviceMac\"] == \"" + macAddress + "\") |> last()";

        Serial.println("Sending POST request...");
        int httpResponseCode = http.POST(query);

        if (httpResponseCode != 200)
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
            http.end();
            CloudStatus = false;
        }
        else
        {
            CloudStatus = true;
            if (httpResponseCode > 0)
            {
                String response = http.getString();
                Serial.println("Response code: " + String(httpResponseCode));
                Serial.println("Response: " + response);

                String result = "";

                for (int i = 1; i < response.length() - 1; i++)
                {
                    char prevChar = response.charAt(i - 1);
                    char currChar = response.charAt(i);
                    char nextChar = response.charAt(i + 1);

                    if ((currChar == '1' || currChar == '0') && prevChar == ',' && nextChar == ',')
                    {
                        result += currChar;
                    }
                }

                Serial.println("Parsed Cloud data:" + result);

                if (result.length() >= 4)
                {
                    String CloudRelayState = String(result.charAt(3));
                    Serial.print("Cloud Relay State: ");
                    Serial.println(CloudRelayState);
                    if (CloudRelayState == "1")
                    {
                        CurrentCloudRelayState = true;
                    }
                    if(CloudRelayState == "0")
                    {
                        CurrentCloudRelayState = false;
                    }
                }
            }
        }
        http.end();
    }
}

// ANY LOCAL CHANGE WILL BE UPDATED ON CLOUD VI ATHIS FUNCTION
void SendRelayStateToCloud(int State)
{
    if (WifiStatus)
    {
        String postData = "MINI_RELAY,DeviceMac=" + DeviceMac + " State=" + String(State);

        String fullUrl = String("https://") + host + WriteUrl;
        Serial.println("Preparing to send HTTP request...");
        Serial.println("Full URL: " + fullUrl);
        Serial.println("POST Data: " + postData);

        http.begin(fullUrl);
        http.addHeader("Authorization", "Token " + String(token));
        http.addHeader("Content-Type", "text/plain");

        int httpResponseCode = http.POST(postData);

        Serial.println("HTTP POST request sent");
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        if (httpResponseCode != 204)
        {
            CloudStatus = false;
        }
        else
        {
            CloudStatus = true;
        }

        http.end();
        Serial.println("HTTP request ended");
    }
}


//UPDATE THE CLOUD THAT RELAY IS ALIVE
void SetDeviceStatusOnCloud() {
    if (WifiStatus) {
        String postData = "MINI_RELAY,DeviceMac=" + DeviceMac + " IsDeviceOnline=1";

        String fullUrl = String("https://") + host + WriteUrl;
        Serial.println("Preparing to send HTTP POST request...");

        http.begin(fullUrl);
        http.addHeader("Authorization", "Token " + String(token));
        http.addHeader("Content-Type", "text/plain");

        int httpResponseCode = http.POST(postData);

        Serial.println("HTTP POST request sent");
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        if (httpResponseCode != 204) {
            Serial.println("Failed to send data to cloud.");
            CloudStatus = false;
        } else {
            Serial.println("Data sent to cloud successfully.");
            CloudStatus = true;
        }

        http.end();
        Serial.println("HTTP request ended");
    } 
}


#endif