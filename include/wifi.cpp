////////////////////////////////////////////////////////////
/////////// Copyright - OCTAVI PAVON MONTOLIU //////////////
///////////        All Rights Reserved        //////////////
///////////        pavon87@gmail.com          //////////////
///////////           -2016-2018-             //////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//////////////////// WIFI CONFIG ///////////////////////////
////////////////////////////////////////////////////////////

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 500; i++) {
      delay(1);
    }
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi Mode = ");
  Serial.println(WiFi.getMode());
  Serial.print("WiFi PhyMode = ");
  Serial.println(WiFi.getPhyMode());
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("STA Status: ");
  Serial.println(WiFi.status());
  Serial.print("STA RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.print("STA psk: ");
  Serial.println(WiFi.psk());
  Serial.print("STA BSSID: ");
  Serial.println(WiFi.BSSIDstr());
  Serial.print("STA Chanel: ");
  Serial.println(WiFi.channel());
  Serial.println(WiFi.localIP());
  Serial.println("");
}

//OTA UPDATES OF CODE VIA WIFI
void ota_update() {
  ArduinoOTA.setHostname(id);
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.onStart([]() {
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

