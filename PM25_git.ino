#include "PMS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
// WIRE TMP to pin D5 (GPIO14)
#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensors(&oneWire);

const char* ssid = "***";
const char* password =  "***";
const char* mqttServer = "192.168.1.***";
const int mqttPort = 1883;
const char* mqttUser = "***";
const char* mqttPassword = "***";
unsigned long time_now = 0;
//holding values for PM
int pm1val = 0;
int pm25val = 0;
int pm10val = 0;


WiFiClient espClient;
PubSubClient client(espClient);


PMS pms(Serial);
PMS::DATA data;

void setup()
{
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial1.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)
  sensors.begin(); 

  
  WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
 
// Create a random client ID
    String clientId = "pm25temp_";
    clientId += String(random(0xffff), HEX);
if (client.connect(clientId.c_str())) {    
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
}
void reconnect() {
  Serial.println("Reconnect activated");
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
String clientId = "pm25_temp_";
    clientId += String(random(0xffff), HEX);
if (client.connect(clientId.c_str())) {    
      Serial.println("connected");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  //read PM data
  if (pms.read(data))
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    pm1val = data.PM_AE_UG_1_0;
    // client.publish("esp/pm1", String(data.PM_AE_UG_1_0).c_str());

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    pm25val = data.PM_AE_UG_2_5;
    //client.publish("esp/pm25", String(data.PM_AE_UG_2_5).c_str());

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    pm10val = data.PM_AE_UG_10_0;
     //client.publish("esp/pm10", String(data.PM_AE_UG_10_0).c_str());
  }
  //report data every 10mins (600,000ms)
  
if(millis() > time_now + 600000){
  //get temp
   sensors.requestTemperatures(); // Send the command to get temperature readings 
   Serial.print(sensors.getTempCByIndex(0)); 
    client.publish("esp/tempout", String(sensors.getTempCByIndex(0)).c_str());
    //send pm data
    client.publish("esp/pm1", String(pm1val).c_str());
    client.publish("esp/pm25", String(pm25val).c_str());
    client.publish("esp/pm10", String(pm10val).c_str());
  time_now = millis();
 
  }

client.loop();
}
