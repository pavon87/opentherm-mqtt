////////////////////////////////////////////////////////////
/////////////////// MQTT FUNCTIONS /////////////////////////
////////////////////////////////////////////////////////////

//MQTT RECEIVER AND CASE SELECT

//MQTT RECEIVER AND CASE SELECT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  //THE VALUES OF TOPICIS BEEN BETWEEN 49 AND 53...
  int topicId = ((topic[3]) - 48);
  payload[length] = '\0';
  String payloadS = String((char*)payload);
  float payloadF = payloadS.toFloat();
  char payloadString[10];
  strcpy(payloadString, ((char*)payload));

  Serial.print("Message arrived. Topic [");
  Serial.print(topic);
  Serial.print("] Payload [");
  Serial.print(payloadF);
  Serial.print("]");

  if (topic[7] == 'i') {
    char idmcu[15];
    char message[30];
    byte mac[6];
    WiFi.macAddress(mac);
    sprintf(idmcu, "%02X%02X%02X\0", mac[3], mac[4], mac[5]);
    strcpy(message, idmcu);

    int ncase = ((((char)payload[0]) - '0') * 10) + (((char)payload[1]) - '0');
    switch (ncase) {
      case 0:
        break;
      case 1:
        enableCentralHeating = true;
        Serial.println("Heating system enabled");
        client.publish(heatingEnabledTopic, "1", true);
        EEPROM.put(10, 1);
        break;
      case 2:
        enableHotWater = true;
        Serial.println("ACS system enabled");
        client.publish(acsEnabledTopic, "1", true);
        EEPROM.put(20, 1);
        break;
      case 10:
        enableCentralHeating = false;
        Serial.println("Heating system enabled");
        client.publish(heatingEnabledTopic, "0", true);
        EEPROM.put(10, 0);
        break;
      case 20:
        enableHotWater = false;
        Serial.println("ACS system disabled");
        client.publish(acsEnabledTopic, "0", true);
        EEPROM.put(20, 0);
        break;
      case 99:
        ESP.restart();
        break;     
    }
    }
  // GET ROOM TEMPS
  if (topic[10] == 'r') {
    roomTemp = payloadF;
    EEPROM.put(30, roomTemp);
    client.publish(roomTempTopic, payloadString, true);
  }
  // SET TARGET TEMPS
  if (topic[10] == 's') {
    setPoint = payloadF;
    EEPROM.put(40, setPoint);
    client.publish(setPointTopic, payloadString, true);
  }
  // SET TARGET TEMPS ACS
  if (topic[10] == 'a') {
    acsTemp = payloadF;
    EEPROM.put(50, acsTemp);
    client.publish(setAcsTempTopic, payloadString, true);
  }
  EEPROM.commit();
}

//MQTT TOPIC COMPOSITION WITH MAC ADDRESS
void init_id() {
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(id, "%02X%02X%02X\0", mac[3], mac[4], mac[5]);
  
  //GENERAL
  strcpy(outTopic, id);
  strcat(outTopic, "/out");
  strcpy(inTopic, id);
  strcat(inTopic, "/in");
  strcpy(mqtt_status, id);
  strcat(mqtt_status, "/status");

  //SET TEMPS INPUTS
  strcpy(setSetPointTopic, inTopic);
  strcat(setSetPointTopic, "/set");
  strcpy(setRoomTempTopic, inTopic);
  strcat(setRoomTempTopic, "/room");
  
  strcpy(openThermStatus, outTopic);
  strcat(openThermStatus, "/openThermStatus");

  strcpy(setPointTopic, outTopic);
  strcat(setPointTopic, "/set");
  strcpy(roomTempTopic, outTopic);
  strcat(roomTempTopic, "/room");

  strcpy(heatingEnabledTopic, outTopic);
  strcat(heatingEnabledTopic, "/heatingEnable");

  strcpy(setAcsTempTopic, inTopic);
  strcat(setAcsTempTopic, "/acsTemp");  
  strcpy(acsStateTopic, outTopic);
  strcat(acsStateTopic, "/acsState");
  strcpy(acsTempTopic, outTopic);
  strcat(acsTempTopic, "/acsTemp");
  strcpy(acsEnabledTopic, outTopic);
  strcat(acsEnabledTopic, "/acsEnabled");

  strcpy(boilerAmbTempTopic, outTopic);
  strcat(boilerAmbTempTopic, "/boilerAmbientTemp");
  strcpy(boilerStateTopic, outTopic);
  strcat(boilerStateTopic, "/boilerState");
  strcpy(boilerTempTopic, outTopic);
  strcat(boilerTempTopic, "/boilerTemp");
  strcpy(flameStateTopic, outTopic);
  strcat(flameStateTopic, "/flameState");

  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Topic In: ");
  Serial.println(inTopic);
  Serial.print("Topic Out: ");
  Serial.println(outTopic);
}


//MQTT RECONNECTION IN CASE OF FAULT
void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(id, mqtt_user, mqtt_password, mqtt_status, 0, true, "offline" )) {
      Serial.println("connected");
      client.publish(mqtt_status, "online", true);
      // ... and resubscribe
      client.subscribe(inTopic);
      client.subscribe(setRoomTempTopic);
      client.subscribe(setSetPointTopic);
      client.subscribe(setAcsTempTopic);    
      } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      for (int i = 0; i < 5000; i++) {
        delay(1);
      }
    }
  }
}
