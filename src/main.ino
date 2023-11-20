
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

//WIFI
#define ssid "pavon-wifi"
#define password "diga33wifi"
WiFiClient espClient;
PubSubClient client(espClient);

//MQTT Server
#define mqtt_server "1.2.3.4"
#define mqtt_port "1883"
#define mqtt_user "username"
#define mqtt_password "12345678

//GLOBAL VARIABLES AND MQTT TOPICS
long lastAlive = 0;
char id[15];
char outTopic[30];
char inTopic[30];
char mqtt_status[30];
char roomTempTopic[30];

char setPointTopic[30];
char setSetPointTopic[30];
char setRoomTempTopic[30];

char boilerStateTopic[30];
char boilerTempTopic[30];
char boilerAmbTempTopic[30];
char flameStateTopic[30];

char openThermStatus[30];
char heatingEnabledTopic[30];

char acsStateTopic[30];
char acsEnabledTopic[30];
char setAcsTempTopic[30];
char setSetAcsTempTopic[30];
char acsTempTopic[30];

char message[100];
//OpenTherm input and output wires connected to 4 and 5 pins on the OpenTherm Shield
const int inPin = D1;
const int outPin = D2;

//DS18B20 sensor config
const int oneWirePin = 14;
OneWire oneWireBus(oneWirePin);
DallasTemperature sensor(&oneWireBus);
OpenTherm ot(inPin, outPin);

bool enableCentralHeating;
bool enableHotWater;

float setPoint;//set point /sp
float roomTemp; //current temperature /pv
float roomTempLast; //current temperature /pv
float ierr = 0; //integral error
float dt = 0; //time between measurements
float op = 0; //PID controller output
float acsTemp;
float acsTempGet;
float lastAcsTempGet;
float boilerTemp;
float lastBoilerTemp;
float boilerAmbTemp;
float lastBoilerAmbTemp;

////////////////////////////////////////////////////////////
///////////////////// MAIN PROGRAM /////////////////////////
////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);
  EEPROM.begin(512);

//READ AND SET LAST STATES
enableCentralHeating = EEPROM.read(10);
enableHotWater = EEPROM.read(20);
EEPROM.get(30, roomTemp);
EEPROM.get(40, setPoint);
EEPROM.get(50, acsTemp);

// Start the DS18B20 sensor
	sensor.begin(); 

  setup_wifi();
  ota_update();
  init_id();

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
}

void loop() {
  ArduinoOTA.handle();

  if (!client.connected()) {
    mqtt_reconnect();
  } else {
    client.loop();
  }
  if (millis() - lastAlive > 5000) {
    
    unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater, false);
    //unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
    
    OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
    if (responseStatus != OpenThermResponseStatus::SUCCESS) {
      Serial.println("Error: Invalid boiler response " + String(response, HEX));
      char message[100] = "Error: Invalid boiler response";
      client.publish(openThermStatus, "Error: Invalid boiler response", true);

    }   
    if (responseStatus == OpenThermResponseStatus::NONE) {
      Serial.println("Error: OpenTherm is not initialized");
      char message[100] = "Error: OpenTherm is not initialized";
      client.publish(openThermStatus, "Error: OpenTherm is not initialized", true);
    }
    else if (responseStatus == OpenThermResponseStatus::INVALID) {
      Serial.println("Error: Invalid response " + String(response, HEX));
      char message[100] = "Error: Invalid response";
      client.publish(openThermStatus, "Error: Invalid response", true);
    }
    else if (responseStatus == OpenThermResponseStatus::TIMEOUT) {
      Serial.println("Error: Response timeout");
      char message[100] = "Error: Response timeout";
      client.publish(openThermStatus, "Error: Response timeout", true);
    }

    if (responseStatus == OpenThermResponseStatus::SUCCESS) {
      op = pid(setPoint, roomTemp, roomTempLast, ierr, dt);

      ot.setBoilerTemperature(op);
      ot.setDHWSetpoint(acsTemp);
      
      getBoilerInfo(response);
      
    client.publish(openThermStatus, "Connected", true);

    }
    getBoilerInfo(response);

    roomTempLast = roomTemp; 
    lastAlive = millis();
  }
  
}
