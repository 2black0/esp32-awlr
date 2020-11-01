void wdt_reset() {
  if(serialStatus){
    Serial.println("WDT: Reset");
  }
  pinMode(wdtPin, OUTPUT);
  digitalWrite(wdtPin, LOW);
  delay(100);
  digitalWrite(wdtPin, HIGH);
  pinMode(wdtPin, INPUT);
}
