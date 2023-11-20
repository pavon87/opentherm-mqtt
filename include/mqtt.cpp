////////////////////////////////////////////////////////////
/////////// Copyright - OCTAVI PAVON MONTOLIU //////////////
///////////        All Rights Reserved        //////////////
///////////        pavon87@gmail.com          //////////////
///////////           -2016-2018-             //////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/////////////////// MQTT FUNCTIONS /////////////////////////
////////////////////////////////////////////////////////////

//MQTT RECEIVER AND CASE SELECT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  int ncase = ((((char)payload[0]) - '0') * 10) + (((char)payload[1]) - '0');
  switch (ncase) {
    case 0:
      digitalWrite(relay1, 0);
      digitalWrite(relay0, 1);
      client.publish(relay0_topic, "1");
      client.publish(relay1_topic, "0");
      strcpy(lastRelay_topic, relay0_topic);
      EEPROM.write(10, 1);
      EEPROM.write(11, 0);
      break;
    case 1:
      digitalWrite(relay0, 0);
      digitalWrite(relay1, 1);
      client.publish(relay0_topic, "0");
      client.publish(relay1_topic, "1");
      strcpy(lastRelay_topic, relay1_topic);
      EEPROM.write(11, 1);
      EEPROM.write(10, 0);
      break;
    case 2:
      digitalWrite(relay3, 0);
      digitalWrite(relay2, 1);
      client.publish(relay2_topic, "1");
      client.publish(relay3_topic, "0");
      strcpy(lastRelay_topic, relay2_topic);
      EEPROM.write(12, 1);
      EEPROM.write(13, 0);
      break;
    case 3:
      digitalWrite(relay2, 0);
      digitalWrite(relay3, 1);
      client.publish(relay3_topic, "1");
      client.publish(relay2_topic, "0");
      strcpy(lastRelay_topic, relay3_topic);
      EEPROM.write(13, 1);
      EEPROM.write(12, 0);
      break;
    case 4:
      digitalWrite(relay4, 0);
      client.publish(relay4_topic, "1");
      strcpy(lastRelay_topic, relay4_topic);
      EEPROM.write(14, 1);
      break;
    case 5:
      digitalWrite(relay5, 0);
      client.publish(relay5_topic, "1");
      strcpy(lastRelay_topic, relay5_topic);
      EEPROM.write(15, 1);
      break;
    case 6:
      digitalWrite(relay6, 0);
      client.publish(relay6_topic, "1");
      strcpy(lastRelay_topic, relay6_topic);
      EEPROM.write(16, 1);
      break;
    case 7:
      temphum();
      break;
    case 8:
      raining();
      break;
    case 9:
      break;
    case 10:
      break;
    case 11:
      break;
    case 12:
      digitalWrite(relay0, 0);
      digitalWrite(relay1, 0);
      client.publish(relay0_topic, "0");
      client.publish(relay1_topic, "0");
      EEPROM.write(10, 0);
      EEPROM.write(11, 0);
      break;
    case 13:
      digitalWrite(relay2, 0);
      digitalWrite(relay3, 0);
      client.publish(relay2_topic, "0");
      client.publish(relay3_topic, "0");
      EEPROM.write(12, 0);
      EEPROM.write(13, 0);
      break;
    case 14:
      digitalWrite(relay4, 1);
      client.publish(relay4_topic, "0");
      EEPROM.write(14, 0);
      break;
    case 15:
      digitalWrite(relay5, 1);
      client.publish(relay5_topic, "0");
      EEPROM.write(15, 0);
      break;
    case 16:
      digitalWrite(relay6, 1);
      client.publish(relay6_topic, "0");
      EEPROM.write(16, 0);
      break;

      /*
        case 20:
            digitalWrite(relay0, 0);
            client.publish(relay0_topic, "0");
            break;
          case 21:
            digitalWrite(relay1, 0);
            client.publish(relay1_topic, "0");
            break;
          case 22:
            digitalWrite(relay2, 0);
            client.publish(relay2_topic, "0");
            break;
          case 23:
            digitalWrite(relay3, 0);
            client.publish(relay3_topic, "0");
            break;
      */
  }
  EEPROM.commit();
}

//MQTT TOPIC COMPOSITION WITH MAC ADDRESS
void init_id() {
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(id, "%02X%02X%02X\0", mac[3], mac[4], mac[5]);
  strcpy(outTopic, id);
  strcat(outTopic, "/out");
  strcpy(inTopic, id);
  strcat(inTopic, "/in");
  strcpy(mqtt_status, id);
  strcat(mqtt_status, "/status");
  strcpy(relay0_topic, outTopic);
  strcat(relay0_topic, "/relay0");
  strcpy(relay1_topic, outTopic);
  strcat(relay1_topic, "/relay1");
  strcpy(relay2_topic, outTopic);
  strcat(relay2_topic, "/relay2");
  strcpy(relay3_topic, outTopic);
  strcat(relay3_topic, "/relay3");
  strcpy(relay4_topic, outTopic);
  strcat(relay4_topic, "/relay4");
  strcpy(relay5_topic, outTopic);
  strcat(relay5_topic, "/relay5");
  strcpy(relay6_topic, outTopic);
  strcat(relay6_topic, "/relay6");
  strcpy(drops_topic, outTopic);
  strcat(drops_topic, "/raining");
  strcpy(humidity_topic, outTopic);
  strcat(humidity_topic, "/hum");
  strcpy(temperature_topic, outTopic);
  strcat(temperature_topic, "/temp");

  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Topic In: ");
  Serial.println(inTopic);
  Serial.print("Topic Out: ");
  Serial.println(outTopic);
}

//MQTT RECONNECTION IN CASE OF FAULT
void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(id, mqtt_user, mqtt_password )) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "Board booted");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
