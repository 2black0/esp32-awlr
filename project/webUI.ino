Card dates(&dashboard, GENERIC_CARD, "Date");
Card times(&dashboard, GENERIC_CARD, "Time");
Card batvolt(&dashboard, GENERIC_CARD, "Battery Voltage (C0)", "V");
Card irradiance(&dashboard, GENERIC_CARD, "Irradiance (C1)", "W/m2");
Card airpress(&dashboard, GENERIC_CARD, "Air Pressure (C2)", "hPa");
Card tempina(&dashboard, GENERIC_CARD, "Temperature In 1 (C3)", "°C");
Card tempinb(&dashboard, GENERIC_CARD, "Temperature In 2 (C4)", "°C");
Card tempout(&dashboard, GENERIC_CARD, "Temperature Out (C5)", "°C");
Card humout(&dashboard, GENERIC_CARD, "Humidity Out (C6)", "%");
Card rainfall(&dashboard, GENERIC_CARD, "Rainfall (C7)", "Inch");
Card windspeed(&dashboard, GENERIC_CARD, "Wind Speed (C8)", "miles/h");
Card winddir(&dashboard, GENERIC_CARD, "Wind Direction (C9)");
Card ultrasonica(&dashboard, GENERIC_CARD, "Ultrasonic 1 (C10)", "cm");
Card waterlevela(&dashboard, GENERIC_CARD, "Water Level 1 (C11)", "cm");
Card ultrasonicb(&dashboard, GENERIC_CARD, "Ultrasonic 2 (C12)", "cm");
Card waterlevelb(&dashboard, GENERIC_CARD, "Water Level 2 (C13)", "cm");

Card apikey(&dashboard, SLIDER_CARD, "Api Key", "", 1, 75);
Card batcalib(&dashboard, SLIDER_CARD, "Bat Calibration", "mV", 11000, 13000);
Card sendtime(&dashboard, SLIDER_CARD, "Sending Time", "Min", 0, 30);
Card ultrasonicgrounda(&dashboard, SLIDER_CARD, "Ultrasonic 1 to Ground", "cm", 0, 350);
Card ultrasonicgroundb(&dashboard, SLIDER_CARD, "Ultrasonic 2 to Ground", "cm", 0, 350);

Card sendtest(&dashboard, BUTTON_CARD, "Sending Test");
Card serialdebug(&dashboard, BUTTON_CARD, "Serial Debug");


void function_callback(){
  apikey.attachCallback([&](int value){
    if(serialStatus){
      Serial.println("[apikey] Slider Callback Triggered: "+String(value));
    }
    apikey.update(value);
    apiVal = value;
    String inputMessage = String(value);
    writeFile(SPIFFS, "/apiVal.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });
  
  batcalib.attachCallback([&](int value){
    if(serialStatus){
      Serial.println("[batcalib] Slider Callback Triggered: "+String(value));
    }
    batcalib.update(value);
    batCalibVal = value;
    getADCBattery();
    fbatCalibVal = (float)adcVal / batCalibVal;
    String inputMessage = String(batCalibVal);
    writeFile(SPIFFS, "/batCalibVal.txt", inputMessage.c_str());
    inputMessage = String(fbatCalibVal);
    writeFile(SPIFFS, "/fbatCalibVal.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });
  
  sendtime.attachCallback([&](int value){
    if(serialStatus){
      Serial.println("[sendtime] Slider Callback Triggered: "+String(value));
    }
    sendtime.update(value);
    sendTimeVal = value;
    sendTimeVals = sendTimeVal * 2 - 1;
    if(sendTimeVals < 0) sendTimeVals = 0;
    String inputMessage = String(sendTimeVal);
    writeFile(SPIFFS, "/sendTimeVal.txt", inputMessage.c_str());
    inputMessage = String(sendTimeVals);
    writeFile(SPIFFS, "/sendTimeVals.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });
  
  ultrasonicgrounda.attachCallback([&](int value){
    if(serialStatus){
      Serial.println("[ultrasonicgrounda] Slider Callback Triggered: "+String(value));
    }
    ultrasonicgrounda.update(value);
    ultrasonicGroundAVal = value;
    String inputMessage = String(ultrasonicGroundAVal);
    writeFile(SPIFFS, "/ultrasonicGroundAVal.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });

  ultrasonicgroundb.attachCallback([&](int value){
    if(serialStatus){
      Serial.println("[ultrasonicgroundb] Slider Callback Triggered: "+String(value));
    }
    ultrasonicgroundb.update(value);
    ultrasonicGroundBVal = value;
    String inputMessage = String(ultrasonicGroundBVal);
    writeFile(SPIFFS, "/ultrasonicGroundBVal.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });

  sendtest.attachCallback([&](bool value){
    if(serialStatus){
      Serial.println("[sendtest] Button Callback Triggered: "+String((value)?"true":"false"));
    }
    sendtest.update(value);
    dashboard.sendUpdates();
    if(value){
      simStatus = value;
      String inputMessage = String(simStatus);
      writeFile(SPIFFS, "/simStatus.txt", inputMessage.c_str());
      ESP.restart();
    }
  });

  serialdebug.attachCallback([&](bool value){
    if(serialStatus){
      Serial.println("[serialdebug] Button Callback Triggered: "+String((value)?"true":"false"));
    }
    serialdebug.update(value);
    serialStatus = value;
    String inputMessage = String(value);
    writeFile(SPIFFS, "/serialStatus.txt", inputMessage.c_str());
    dashboard.sendUpdates();
  });
}

void update_webui() {
  /*dates.update(sdatenowVal);
  times.update(stimenowVal);*/
  batvolt.update(fbatteryVal);
  irradiance.update(firradianceVal);
  airpress.update(fairpressureVal);
  tempina.update(ftemperatureBmpVal);
  tempinb.update(ftemperatureRtcVal);
  tempout.update(ftemperatureHdcVal);
  humout.update(fhumidityHdcVal);
  rainfall.update(rainVal);
  windspeed.update(windSpeedVal);
  winddir.update(windDirectionVal);
  ultrasonica.update(ultrasonicAVal);
  waterlevela.update(waterLevelAVal);
  ultrasonicb.update(ultrasonicBVal);
  waterlevelb.update(waterLevelBVal);

  apikey.update(apiVal);
  batcalib.update(batCalibVal);
  sendtime.update(sendTimeVal);
  ultrasonicgrounda.update(ultrasonicGroundAVal);
  ultrasonicgroundb.update(ultrasonicGroundBVal);
  
  serialdebug.update(serialStatus);
}
