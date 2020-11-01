void clrRainWind() {
  rainTotalVal = 0;
  srainTotalVal = String(rainTotalVal);
  writeFile(SPIFFS, "/rainTotalVal.txt", srainTotalVal.c_str());
  windSpeedTotalVal = 0;
  swindSpeedTotalVal = String(windSpeedTotalVal);
  writeFile(SPIFFS, "/windTotalVal.txt", swindSpeedTotalVal.c_str());
}

void showSensor() {
  if(serialStatus){
    Serial.println(F("************************"));
    Serial.print(F("Send Count: "));
    Serial.println(sendCountVal);
    Serial.print(F("Date: "));
    Serial.println(sdatenowVal);
    Serial.print(F("Time: "));
    Serial.println(stimenowVal);
    Serial.print(F("Battery: "));
    Serial.println(sbatteryVal);
    Serial.print(F("Irradiance: "));
    Serial.println(sirradianceVal);
  
    Serial.print(F("Air Pressure: "));
    Serial.println(sairpressureVal);
    Serial.print(F("BMP Temperature: "));
    Serial.println(stemperatureBmpVal);
    Serial.print(F("RTC Temperature: "));
    Serial.println(stemperatureRtcVal);
    Serial.print(F("HDC Temperature: "));
    Serial.println(stemperatureHdcVal);
    Serial.print(F("HDC Humidity: "));
    Serial.println(shumidityHdcVal);
  
    Serial.print(F("Rainfall: "));
    Serial.println(srainVal);
    Serial.print(F("Wind Speed: "));
    Serial.println(swindSpeedVal);
    Serial.print(F("Wind Direction: "));
    Serial.println(swindDirectionVal);
    Serial.print(F("Water Level A: "));
    Serial.println(swaterLevelAVal);
    Serial.print(F("Water Level B: "));
    Serial.println(swaterLevelBVal);
    Serial.println(F("************************"));
  }
}

void updateSensor() {
  if(serialStatus){
    Serial.println("Sensor: Update");
  }
  sbatCalibVal = String(batCalibVal);
  ssendCountVal = String(sendCountVal);

  getDateTimenow();
  getBattery();
  getIrradiance();
  getPressure();
  getTempBMP();
  getTempRTC();
  getTempHDC();
  getHumHDC();
  getRainfall();
  getWindSpeed();
  getWindDirection();
  getUltrasonicA();
  getUltrasonicB();
}

String getADCBattery() {
  int16_t adc;
  adcVal = 0;
  for(int i=0; i<9; i++){
    if (bmpStatus) {
      adc = ads.readADC_SingleEnded(0);
    } else {
      adc = analogRead(windPin);
    }
    adcVal = adcVal + adc;
  }
  adcVal = adcVal / 10;
}

String getBattery() {
  int16_t adc;
  adcVal = 0;
  for(int i=0; i<9; i++){
    if (bmpStatus) {
      adc = ads.readADC_SingleEnded(0);
    } else {
      adc = analogRead(windPin);
    }
    adcVal = adcVal + adc;
  }
  adcVal = adcVal / 10;
  fbatteryVal = (float)((adcVal / fbatCalibVal) / 1000);
  sbatteryVal = String(fbatteryVal);
}

String getIrradiance() {
  int16_t adc1;
  if (bmpStatus) {
    adc1 = ads.readADC_SingleEnded(1);
    firradianceVal = pow(adc1 / 5357.7582, 2) * 10000 / 3.75;
  } else {
    firradianceVal = 0;
  }
  sirradianceVal = String(firradianceVal);
}

String getPressure() {
  if (bmpStatus) {
    sensors_event_t pressure_event;
    bmp_pressure->getEvent(&pressure_event);
    fairpressureVal = pressure_event.pressure;
  } else {
    fairpressureVal = 0;
  }
  sairpressureVal = String(fairpressureVal);
}

String getTempBMP() {
  if (bmpStatus) {
    sensors_event_t temp_event;
    bmp_temp->getEvent(&temp_event);
    ftemperatureBmpVal = temp_event.temperature;
  } else {
    ftemperatureBmpVal = 0;
  }
  stemperatureBmpVal = String(ftemperatureBmpVal);
}

String getTempRTC() {
  if (rtcStatus) {
    ftemperatureRtcVal = rtc.getTemperature();
  } else {
    ftemperatureRtcVal = 0;
  }
  stemperatureRtcVal = String(ftemperatureRtcVal);
}

String getTempHDC() {
  if (bmpStatus) {
    ftemperatureHdcVal = hdc1080.readTemperature();
  } else {
    ftemperatureHdcVal = 0;
  }
  stemperatureHdcVal = String(ftemperatureHdcVal);
}

String getHumHDC() {
  if (bmpStatus) {
    fhumidityHdcVal = hdc1080.readHumidity();
  } else {
    fhumidityHdcVal = 0;
  }
  shumidityHdcVal = String(fhumidityHdcVal);
}

String getRainfall() {
  if (!bmpStatus || rainVal < 1) {
    rainVal = 0;
  }
  rainTotalVal = readFile(SPIFFS, "/rainTotalVal.txt").toInt();
  rainVal = rainVal + rainTotalVal;
  srainVal = String(rainVal);
  writeFile(SPIFFS, "/rainTotalVal.txt", srainVal.c_str());

  if (sendCountVal > sendTimeVals) {
    float frainVal = rainVal * 0.2794;
    srainVal = String(frainVal);
    rainTotalVal = 0;
    srainTotalVal = String(rainTotalVal);
    writeFile(SPIFFS, "/rainTotalVal.txt", srainTotalVal.c_str());
  }
}

String getWindSpeed() {
  if (!bmpStatus || windSpeedVal < 1) {
    windSpeedVal = 0;
  }
  windSpeedTotalVal = readFile(SPIFFS, "/windTotalVal.txt").toInt();
  windSpeedVal = windSpeedVal + windSpeedTotalVal;
  swindSpeedVal = String(windSpeedVal);
  writeFile(SPIFFS, "/windTotalVal.txt", swindSpeedVal.c_str());

  if (sendCountVal > sendTimeVals) {
    float fwindSpeedVal = windSpeedVal / 300 * 2.4;
    swindSpeedVal = String(fwindSpeedVal);
    windSpeedTotalVal = 0;
    swindSpeedTotalVal = String(windSpeedTotalVal);
    writeFile(SPIFFS, "/windTotalVal.txt", swindSpeedTotalVal.c_str());
  }
}

String getWindDirection() {
  int16_t adc2;
  if (bmpStatus) {
    adc2 = ads.readADC_SingleEnded(2);
    windDirectionVal = adc2;
  } else {
    windDirectionVal = 0;
  }
  if (windDirectionVal == 0) {
    windDirectionVal = 0;
  } else if (windDirectionVal > 15000 && windDirectionVal < 15500) {
    windDirectionVal = 1;
  } else if (windDirectionVal > 16000 && windDirectionVal < 16500) {
    windDirectionVal = 2;
  } else if (windDirectionVal > 16750 && windDirectionVal < 17250) {
    windDirectionVal = 3;
  } else if (windDirectionVal > 13250 && windDirectionVal < 13750) {
    windDirectionVal = 4;
  } else if (windDirectionVal > 7750 && windDirectionVal < 8250) {
    windDirectionVal = 5;
  } else if (windDirectionVal > 5250 && windDirectionVal < 5750) {
    windDirectionVal = 6;
  } else if (windDirectionVal > 2750 && windDirectionVal < 3250) {
    windDirectionVal = 7;
  } else {
    windDirectionVal = 8;
  }
  swindDirectionVal = String(windDirectionVal);
}

String getUltrasonicA() {
  if (bmpStatus) {
    ultrasonicAVal = 0;
  } else {
    digitalWrite(ultrasonicT1Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonicT1Pin, HIGH);
    delayMicroseconds(20);
    digitalWrite(ultrasonicT1Pin, LOW);
    ultrasonicAVal = pulseIn(ultrasonicE1Pin, HIGH, 26000);
    ultrasonicAVal = ultrasonicAVal / 58;
  }
  sultrasonicAVal = String(ultrasonicAVal);
  waterLevelAVal = ultrasonicGroundAVal - ultrasonicAVal;
  swaterLevelAVal = String(waterLevelAVal);
}

String getUltrasonicB() {
  if (bmpStatus) {
    ultrasonicBVal = 0;
  } else {
    digitalWrite(ultrasonicT2Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonicT2Pin, HIGH);
    delayMicroseconds(20);
    digitalWrite(ultrasonicT2Pin, LOW);
    ultrasonicBVal = pulseIn(ultrasonicE2Pin, HIGH, 26000);
    ultrasonicBVal = ultrasonicBVal / 58;
  }
  sultrasonicBVal = String(ultrasonicBVal);
  waterLevelBVal = ultrasonicGroundBVal - ultrasonicBVal;
  swaterLevelBVal = String(waterLevelBVal);
}
