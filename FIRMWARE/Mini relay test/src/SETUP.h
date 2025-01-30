#ifndef SETUP_h
#define SETUP_h

#include <CLOUD.h>
#include <INDICATOR.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Web server instance
WebServer server(80);

// Flag to control WiFi resource access
bool WifiResourcesIsBeingUsed = false;

// Static IP configuration
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Define the EEPROM size (adjust if needed)
#define EEPROM_SIZE 512

// Function to set credentials and write to EEPROM
void SetCredentials(String SSID, String Password)
{
    // Clear EEPROM
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }

    // Create JSON string
    DynamicJsonDocument doc(256);
    doc["ssid"] = SSID;
    doc["password"] = Password;

    String jsonString;
    serializeJson(doc, jsonString);

    // Write JSON string to EEPROM
    for (int i = 0; i < jsonString.length(); i++)
    {
        EEPROM.write(i, jsonString[i]);
    }
    EEPROM.write(jsonString.length(), '\0'); // Null terminator

    // Commit changes
    EEPROM.commit();
}

// Function to get credentials from EEPROM
void GetCredentials()
{
    // Read JSON string from EEPROM
    String jsonString = "";
    char ch;

    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        ch = EEPROM.read(i);
        if (ch == '\0')
            break; // End of JSON string
        jsonString += ch;
    }

    // Print the JSON string to Serial Monitor
    Serial.println("Read JSON from EEPROM: " + jsonString);

    // Parse JSON string
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error)
    {
        Serial.print("DeserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Update global credentials
    Credentials.SSID = doc["ssid"].as<String>();
    Credentials.PASSWORD = doc["password"].as<String>();

    Serial.println("Stored SSID: " + Credentials.SSID);
    Serial.println("Stored Password: " + Credentials.PASSWORD);

    if (Credentials.SSID != "" && Credentials.PASSWORD != "")
    {
        OnBoardCredentialsFound = true;
    }
    else
    {
        OnBoardCredentialsFound = false;
    }
}

void SetupDevice()
{
    // Function to handle the root web page
    auto handleRoot = []()
    {
        String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MINI RELAY SETUP</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Nunito:ital,wght@0,200..1000;1,200..1000&display=swap');
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Nunito', Arial, sans-serif;
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #000;
        }

        .container {
            background: #ffffff;
            padding: 40px;
            border-radius: 30px;
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.5);
            max-width: 1000px;
            text-align: center;
        }

        h1 {
            margin-bottom: 30px;
            color: #000;
            font-size: 2.5rem;
        }

        p {
            margin-bottom: 30px;
            color: #000;
            font-size: 1.2rem;
        }

        .input-group {
            margin-bottom: 20px;
            text-align: left;
        }

        label {
            display: block;
            margin-bottom: 10px;
            color: #000;
            font-size: 1.2rem;
        }

        input[type="text"],
        input[type="password"] {
            width: calc(100% - 40px);
            padding: 12px;
            border: 2px solid #000;
            border-radius: 30px;
            background: #000;
            color: #fff;
            font-size: 1.2rem;
            display: inline-block;
        }

        input[type="text"]::placeholder,
        input[type="password"]::placeholder {
            color: #2e2d2d;
        }

        .password-container {
            position: relative;
            display: inline-block;
            width: 100%;
        }

        .eye-icon {
            position: absolute;
            right: 20px;
            top: 50%;
            transform: translateY(-50%);
            cursor: pointer;
            font-size: 3.2rem;
        }

        button {
            padding: 15px;
            background-color: #fff;
            color: #000;
            border: 4px solid #000;
            border-radius: 30px;
            cursor: pointer;
            width: 100%;
            font-size: 1.5rem;
        }

        button:hover {
            background-color: #000;
            color: #fff;
            border: 4px solid #fff;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>MINI RELAY SETUP</h1>
        <p>Device MAC Address: %MAC%</p>
        <form action="/config" method="POST">
            <div class="input-group">
                <label for="ssid">Wi-Fi SSID:</label>
                <input type="text" id="ssid" name="ssid" required>
            </div>
            <div class="input-group">
                <label for="password">Wi-Fi Password:</label>
                <div class="password-container">
                    <input type="password" id="password" name="password">
                    <span class="eye-icon" onclick="togglePassword()">🙈</span>
                </div>
            </div>
            <button type="submit">CONFIGURE DEVICE</button>
        </form>
    </div>
    <script>
        function togglePassword() {
            const passwordField = document.getElementById('password');
            const eyeIcon = document.querySelector('.eye-icon');
            if (passwordField.type === 'password') {
                passwordField.type = 'text';
                eyeIcon.textContent = '🙉';
            } else {
                passwordField.type = 'password';
                eyeIcon.textContent = '🙈';
            }
        }
    </script>
</body>
</html>
)rawliteral";

        String macAddress = WiFi.macAddress();
        html.replace("%MAC%", macAddress);
        server.send(200, "text/html", html);
    };

    // Function to handle configuration form submissions
    auto handleConfig = []()
    {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        Serial.println("Received SSID: " + ssid);
        Serial.println("Received Password: " + password);

        SetCredentials(ssid, password);
        ESP.restart();

        WiFi.softAPdisconnect(true);
    };

    // Function to handle not found pages
    auto handleNotFound = []()
    {
        server.send(404, "text/html", "<h1>404 Not Found</h1>");
    };


    WiFi.disconnect(true);
    delay(100);

    Serial.println("Starting Access Point...");
    WiFi.softAP("MINIRELAY", "PASSWORD");

    if (!WiFi.softAPConfig(local_IP, gateway, subnet))
    {
        Serial.println("Failed to configure static IP");
    }
    else
    {
        Serial.println("Static IP configured");
    }

    Serial.println("Access Point Started");



    // Set up server routes
    server.on("/", handleRoot);
    server.on("/config", HTTP_POST, handleConfig);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Web server started");
}

void SetupEEPROM()
{
    EEPROM.begin(EEPROM_SIZE);
    GetCredentials();
}

#endif