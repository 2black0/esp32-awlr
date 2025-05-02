String readFile(fs::FS &fs, const char *path) {
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    if(serialStatus){
      Serial.println("Error: File not Open");
    }
    return String();
  }
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  if(serialStatus){
    Serial.printf("SPIFFS: Reading file %s ", path);
    Serial.println(fileContent);
  }
  return fileContent;
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  File file = fs.open(path, "w");
  if (!file) {
    if(serialStatus){
      Serial.println("Error: File not Open");
    }
    return;
  }
  if (file.print(message)) {
    if(serialStatus){
      Serial.printf("%s", message);
      Serial.printf(" Written into %s\r\n", path);
    }
  } else {
    if(serialStatus){
      Serial.println("Error: Write Failed");
    }
  }
}
