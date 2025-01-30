# MINI-SMART-RELAY
A updated version of elevate homes,with a better design for convinient usage


## Configuration for  project

### 1. **INFLUX SETUP**

To start using InfluxDB with the Mini Relay project, follow these steps:

1. **Create an InfluxDB Account:**
   - Sign up for an account at [InfluxDB Cloud](https://cloud2.influxdata.com/) if you don't already have one.

2. **Create a Bucket:**
   - Once logged in, create a bucket with the name `MINI_RELAY`.

3. **Get Your API Token and Organization ID:**
   - Generate an API token and note down your organization ID for later use.

After setting up your InfluxDB account and creating the `MINI_RELAY` bucket, modify the `Mini_Relay_Test/src/CLOUD.h` file as described in the next section.

### 2. **Modify the Host:**
Before running the Mini Relay project, you need to modify some details in the `Mini_Relay_Test/src/CLOUD.h` file to properly connect to your InfluxDB instance. Below are the steps to configure it:
```cpp
const char *host = "enter your influx host";
const char *url = "/api/v2/query";
const char *token = "enter your acess token";
//create a bucket with the name "MINI_RELAY"
const char *WriteUrl = "/api/v2/write?org=your org id&bucket=MINI_RELAY";
```

### 3. **WIFI SETUP:**
Once you power on the relay, press and hold the setup button until you see a blue light. Now, on your phone, look for a Wi-Fi network with the SSID "MINIRELAY". Enter "PASSWORD" as the password. Now, open your browser and go to "http://192.168.4.1/" to set the new SSID and password for the relay to connect to.

After that, reset the device, and the relay will connect to your specified network.

1. A red light indicates no network connection.
2. Based on your Wi-Fi signal strength, the color of the indicator changes from green to yellow (green means good signal).
3. If a cloud request fails, you will see a purple blink occasionally.

### 4. **DASHBOARD SETUP:"
Use the webdashboard to acess the relayy over the internet,make sure  you moidify the follwoing in the js code and enter your influx api token,endpoint url,orgid properly


   
