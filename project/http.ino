void build_url() {
  String sstr;
  
  sstr += "api.sipasi.online/?key=";
  sstr += apikeyVal;
  sstr += C00;
  sstr += sbatteryVal;
  sstr += C01;
  sstr += sirradianceVal;
  sstr += C02;
  sstr += sairpressureVal;
  sstr += C03;
  sstr += stemperatureBmpVal;
  sstr += C04;
  sstr += stemperatureRtcVal;
  sstr += C05;
  sstr += stemperatureHdcVal;
  sstr += C06;
  sstr += shumidityHdcVal;
  sstr += C07;
  sstr += srainVal;
  sstr += C08;
  sstr += swindSpeedVal;
  sstr += C09;
  sstr += swindDirectionVal;
  sstr += C10;
  sstr += sultrasonicAVal;
  sstr += C11;
  sstr += swaterLevelAVal;
  sstr += C12;
  sstr += sultrasonicBVal;
  sstr += C13;
  sstr += swaterLevelBVal;
  sstr += C14;
  sstr += sunixnowVal;

  sstr.toCharArray(str, 215);
}

void send_sim() {
  build_url();
  int count = 0;
  int retrycount = 0;

setupretry:
  sim800l->reset();
  count++;
  if (count > 3)
    ESP.restart();

  while (!sim800l->isReady()) {
    if(serialStatus){
      Serial.println(F("SIM: AT Problem"));
    }
    delay(1000);
  }

  while (!sim800l->isReady()) {
    if(serialStatus){
      Serial.println(F("SIM: AT Problem"));
    }
    delay(1000);
  }

  bool lowPowerMode = sim800l->setPowerMode(NORMAL);
  if (lowPowerMode) {
    if(serialStatus){
      Serial.println(F("SIM: Normal Power Mode"));
    }
  } else {
    if(serialStatus){
      Serial.println(F("SIM: Failed Normal Power Mode"));
    }
    goto setupretry;
  }

  uint8_t signal = sim800l->getSignal();
  while (signal <= 0) {
    delay(1000);
    signal = sim800l->getSignal();
  }
  if(serialStatus){
    Serial.print(F("SIM: Signal "));
    Serial.println(signal);
  }
  delay(1000);

  bool success = sim800l->setupGPRS(APN);
  while (!success) {
    success = sim800l->setupGPRS(APN);
    delay(5000);
  }
  if(serialStatus){
    Serial.println(F("SIM: GPRS OK"));
  }
  bool connected = false;
  for (uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    connected = sim800l->connectGPRS();
  }
  if (!connected) {
    goto setupretry;
  }

  uint16_t rc = sim800l->doGet(str, 10000);
  if (rc == 200) {
    if(serialStatus){
      Serial.println(F("SIM: HTTP GET OK"));
      //Serial.print(sim800l->getDataSizeReceived());
      //Serial.println(F(" bytes)"));
      Serial.print(F("SIM: "));
      Serial.println(sim800l->getDataReceived());
    }
  } else {
    if(serialStatus){
      Serial.print(F("SIM: HTTP GET Error "));
      Serial.println(rc);
    }
    goto setupretry;
  }

  while (!sim800l->isReady()) {
    if(serialStatus){
      Serial.println(F("SIM: AT Problem"));
    }
    delay(1000);
  }

  lowPowerMode = sim800l->setPowerMode(SLEEP);
  if (lowPowerMode) {
    if(serialStatus){
      Serial.println(F("SIM: Sleep Power Mode"));
    }
  } else {
    if(serialStatus){
      Serial.println(F("SIM: Failed Sleep Power Mode"));
    }
  }
}
