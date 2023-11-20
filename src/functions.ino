// TEMPERATURE
void getBoilerInfo(unsigned long response) {

  //TEMP INSIDE BOILER
  char temp [10];
  sensor.requestTemperatures(); 
  sprintf (temp, "%.2f", sensor.getTempCByIndex(0));
  boilerAmbTemp = sensor.getTempCByIndex(0);
  if (boilerAmbTemp != lastBoilerAmbTemp){
    client.publish(boilerAmbTempTopic, temp);
  }
  Serial.print("BOILER AMBIENTAL TEMPERATURE: ");
  Serial.print(temp);
  Serial.print("ºC");
  Serial.println(" ");	
  lastBoilerAmbTemp = boilerAmbTemp;

  //BOILER STATUS
  if (ot.isCentralHeatingActive(response) == true) {
    Serial.println("Central Heating is on");
    client.publish(boilerStateTopic, "1", true);
  } else {
    Serial.println("Central Heating is off");
    client.publish(boilerStateTopic, "0", true);
  }
  if (ot.isHotWaterActive(response) == true) {
    Serial.println("Hot Water is on");
    client.publish(acsEnabledTopic, "1", true);
  } else {
    Serial.println("Hot Water is off");
    client.publish(acsEnabledTopic, "0", true);
  }
  if (ot.isFlameOn(response) == true) {
    Serial.println("Flame is on");
    client.publish(flameStateTopic, "1", true);
  } else {
    Serial.println("Flame is off");
    client.publish(flameStateTopic, "0", true);
  }

	//Get Boiler Temperature
	boilerTemp = ot.getBoilerTemperature();
	Serial.print("Boiler temperature is ");	
	Serial.print(boilerTemp);	
	Serial.print("ºC");	
  Serial.println(" ");	
  char boilerTempChar[10];
  sprintf(boilerTempChar, "%f", boilerTemp);
  if (boilerTemp != lastBoilerTemp){
    client.publish(boilerTempTopic, (boilerTempChar), true);
  }
  lastBoilerTemp = boilerTemp;

  //Get ACS Temperature
	acsTempGet = ot.getDHWTemperature();
	Serial.print("ACS temperature is ");	
	Serial.print(acsTempGet);	
	Serial.print("ºC");	
	Serial.println(" ");	
  char acsTempGetChar[10];
  sprintf(acsTempGetChar, "%f", acsTempGet);
  if (lastAcsTempGet != acsTempGet){
    client.publish(acsTempTopic, (acsTempGetChar), true);
  }
  lastAcsTempGet = acsTempGet;
}

void ICACHE_RAM_ATTR handleInterrupt() {
  ot.handleInterrupt();
}

float pid(float setPoint, float roomTemp, float roomTempLast, float& ierr, float dt) {
  float Kc = 10.0; // K / %Heater
  float tauI = 50.0; // sec
  float tauD = 1.0;  // sec
  // PID coefficients
  float KP = Kc;
  float KI = Kc / tauI;
  float KD = Kc*tauD; 
  // upper and lower bounds on heater level
  float ophi = 100;
  float oplo = 0;
  // calculate the error
  float error = setPoint - roomTempLast;
  // calculate the integral error
  ierr = ierr + KI * error * dt;  
  // calculate the measurement derivative
  float dpv = (roomTemp - roomTempLast) / dt;
  // calculate the PID output
  float P = KP * error; //proportional contribution
  float I = ierr; //integral contribution
  float D = -KD * dpv; //derivative contribution
  float op = P + I + D;
  // implement anti-reset windup
  if ((op < oplo) || (op > ophi)) {
    I = I - KI * error * dt;
    // clip output
    op = max(oplo, min(ophi, op));
  }
  ierr = I; 
  Serial.println("setPoint="+String(setPoint) + " roomTemp=" + String(roomTemp) + " dt=" + String(dt) + " op=" + String(op) + " P=" + String(P) + " I=" + String(I) + " D=" + String(D));
  return op;
}