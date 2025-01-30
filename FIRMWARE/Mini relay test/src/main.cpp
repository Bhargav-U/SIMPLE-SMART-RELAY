// ALL IMPORTS THAT ARE NEED
#include <Arduino.h>
#include <INDICATOR.h>
#include <CLOUD.h>
#include <SETUP.h>

// ALL PINS THAT ARE USED
#define RelayPin 17
#define SwitchPin 16
#define SetupPin 18

// TRCAKS THE LOCAL SWITCH STATE
int CurrentSwitchState = 0;
int LastSwitchState = 0;

// SETUP FUNCTION
void setup()
{
  Serial.begin(115200);

  Serial.println("GPIO SETUP");
  pinMode(SwitchPin, INPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(SetupPin, INPUT_PULLUP);

  SetupIndicator();

  SetupEEPROM();

  if (!OnBoardCredentialsFound)
  {
    SetupMode = true;
    ShowStatus();
    SetupDevice();
  }
  else
  {
    Serial.println("credentaisl found on eeprom");
  }

  DeviceMac = WiFi.macAddress();

  delay(1000);
}

// THE LOOP
void loop()
{
  if (!SetupMode)
  {
    if (!WifiStatus)
    {
      Serial.println("Reconnecting to Wifi");
      ConnectToWifi();
    }

    if (digitalRead(SetupPin) == LOW)
    {
      Serial.println("Setup Mode ON");
      SetupMode = true;
      ShowStatus();
      WiFi.disconnect(true);
      delay(100);
      SetupDevice();
    }

    // GET THE CURRENT STATE OF THE SWITCH
    CurrentSwitchState = digitalRead(SwitchPin);

    // IF CURRENT STATE OF THE SWITCH IS DIFFERENT FROM THE LAST ONE UPDATE THE RELAY AND SEND IT TO THE CLOUD
    if (CurrentSwitchState != LastSwitchState)
    {
      Serial.print("Switch state changed: ");
      Serial.println(CurrentSwitchState);

      LastSwitchState = CurrentSwitchState;
      digitalWrite(RelayPin, CurrentSwitchState);

      (CurrentSwitchState == HIGH) ? RelayState = true : RelayState = false;
      (CurrentSwitchState == HIGH) ? SendRelayStateToCloud(1) : SendRelayStateToCloud(0);
    }

    // INCASE IF THE IF LOOP IS SATISFIED THEN AAUTOMATICALLY LOCAL SATTE AND CLOUD STATE MATCH,SO NO NEED TO CHECK FOR CLOUD STATE
    // IF THE IF CONDITION IS NOT SATISFIED THEN CHECK IF ANY CLOUD STATE CHANGED,IF YES,MAKE THE CHNAGES TO RELAY AND THE CLOUD STATE VARIABLES
    else
    {
      GetRelayStateFromCloud();
      SetDeviceStatusOnCloud();
      if (LastCloudRelayState != CurrentCloudRelayState)
      {
        LastCloudRelayState = CurrentCloudRelayState;
        if (CurrentCloudRelayState)
        {
          RelayState = true;
        }
        else
        {
          RelayState = false;
        }
        digitalWrite(RelayPin, CurrentCloudRelayState);
      }
    }

    // THIS WILL SHOW THE STATUS FOR EASY DEBUGGING
    ShowStatus();
    Serial.println("Current Relay State: " + String(RelayState));
  }
  // MANAGE SETUP WEBSERVER CLIENT REQUESTS
  server.handleClient();

  // REMOVE THIS AND USE THREADING,ELSE THIS WILLPROVE YOU THE WORLD'S MOST DUMBEST GUY WITH MICROPENIS!
  delay(100);
}