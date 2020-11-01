#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_periph.h"
#include "soc/sens_reg.h"
#include <stdio.h>

#include "esp32/ulp.h"
#include "ulp_main.h"
#include "ulptool.h"

#include "variable.h"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_BMP280.h>
#include <ClosedCube_HDC1080.h>
#include <RTClib.h>
#include <SIM800L.h>
#include <Wire.h>

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");

static void init_ulp_program(void);
static void update_pulse_count(void);
static void pin_setup(gpio_num_t gpio_num);
static void sleep();

const int SLEEP_MIN = 30;
const int ULP_SLEEP_MS = 10;
const gpio_num_t GPIO_WIND = GPIO_NUM_35;
const gpio_num_t GPIO_RAIN = GPIO_NUM_34;

AsyncWebServer server(80);
ESPDash dashboard(&server);
RTC_DS3231 rtc;
ClosedCube_HDC1080 hdc1080;
Adafruit_BMP280 bmp;
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
Adafruit_ADS1115 ads;
SIM800L *sim800l;

// ---------------------------------
// ---------------------------- init variable
// ---------------------------------
void init_variable() {
  apiVal = readFile(SPIFFS, "/apiVal.txt").toInt();
  batCalibVal = readFile(SPIFFS, "/batCalibVal.txt").toInt();
  fbatCalibVal = readFile(SPIFFS, "/fbatCalibVal.txt").toFloat();
  sendTimeVal = readFile(SPIFFS, "/sendTimeVal.txt").toInt();
  sendTimeVals = readFile(SPIFFS, "/sendTimeVals.txt").toInt();
  ultrasonicGroundAVal = readFile(SPIFFS, "/ultrasonicGroundAVal.txt").toInt();
  ultrasonicGroundBVal = readFile(SPIFFS, "/ultrasonicGroundBVal.txt").toInt();
  serialVal = readFile(SPIFFS, "/serialStatus.txt").toInt();
  simStatus = readFile(SPIFFS, "/simStatus.txt").toInt();
  
  getADCBattery();
  fbatCalibVal = (float)adcVal / batCalibVal;
  String inputMessage = String(fbatCalibVal);
  writeFile(SPIFFS, "/fbatCalibVal.txt", inputMessage.c_str());
  sendTimeVals = sendTimeVal * 2 - 1;
  if(sendTimeVals < 0) sendTimeVals = 0;
  inputMessage = String(sendTimeVals);
  writeFile(SPIFFS, "/sendTimeVals.txt", inputMessage.c_str());
  if(serialVal == 1){
    serialStatus = true;
  } else {
    serialStatus = false;
  }
}

// ---------------------------------
// ---------------------------- init setup
// ---------------------------------
void init_setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, simTXPin, simRXPin);
  delay(1000);
  //sim800l = new SIM800L((Stream *)&Serial2, simRSTPin, 200, 512);
  sim800l = new SIM800L((Stream *)&Serial2, simRSTPin, 255, 512, (Stream *)&Serial);
  Wire.begin();

  rtcStatus = rtc.begin();
  if (!rtcStatus) {
    if(serialStatus){
      Serial.println(F("Error: RTC not Found"));
    } 
  } else {
      if (rtc.lostPower()) {
        if(serialStatus){
          Serial.println("RTC: Update RTC Date & Time");
        }
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }
  }

  spiffsStatus = SPIFFS.begin();
  if (!spiffsStatus) {
    if(serialStatus){
      Serial.println(F("Error: SPIFFS not Found"));
    } 
  } else{
      init_variable();
  }

  bmpStatus = bmp.begin();
  if (!bmpStatus) {
    if(serialStatus){
      Serial.println("Mode: AWLR");
    } else {
        if(serialStatus){
          Serial.println("Mode: AWS");
        }
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2,
                        Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16,
                        Adafruit_BMP280::STANDBY_MS_500);
        ads.begin();
        hdc1080.begin(0x40);
        hdc1080.setResolution(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_14BIT);
      }
  }
}

// ---------------------------------
// ---------------------------- init pin
// ---------------------------------
void init_pin() {
  pinMode(sw1Pin, INPUT);
  pinMode(sw2Pin, INPUT);
  pinMode(sw3Pin, INPUT);
  pinMode(simRSTPin, OUTPUT);
  pinMode(alarmPin, INPUT_PULLUP);
  pinMode(buzzPin, OUTPUT);
  pinMode(wdtPin, OUTPUT);
  pinMode(csPin, OUTPUT);
  pinMode(ultrasonicT1Pin, OUTPUT);
  pinMode(ultrasonicE1Pin, INPUT_PULLUP);
  pinMode(ultrasonicT2Pin, OUTPUT);
  pinMode(ultrasonicE2Pin, INPUT_PULLUP);

  digitalWrite(buzzPin, HIGH);
}

// ---------------------------------
// ---------------------------- init wifi
// ---------------------------------
void init_wifi() {
  WiFi.softAP(ssid, password);
  delay(1000);

  function_callback();
  server.begin();
}

// ---------------------------------
// --------------------------- setup
// ---------------------------------
void setup() {
  btStop();
  init_setup();
  init_pin();

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause != ESP_SLEEP_WAKEUP_TIMER) {
    if(serialStatus){
      printf("No TIMER wakeup, initializing ULP\n");
    } 
    init_ulp_program();
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(2 * 1000000));
    sleep();
  }

  runStatus = digitalRead(sw2Pin);
  if (runStatus) {
    if(serialStatus){
      Serial.println(F("Running Mode: Normal"));
    }
  } else {
    if(serialStatus){
      Serial.println("Running Mode: Maintenance");
    }
    if(simStatus){
      send_sim();
      simStatus = false;
      String inputMessage = String(simStatus);
      writeFile(SPIFFS, "/simStatus.txt", inputMessage.c_str());
      ESP.restart(); 
    } else {
      init_wifi();
    }
  }
}

// ---------------------------------
// ---------------------------- normal loop
// ---------------------------------
void normal_loop() {
  wdt_reset();
  update_pulse_count();
  ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(SLEEP_MIN * 1000000));

  sendCountVal = readFile(SPIFFS, "/sendCountVal.txt").toInt();
  sendCountVal++;
  String sCount = String(sendCountVal);
  writeFile(SPIFFS, "/sendCountVal.txt", sCount.c_str());

  updateSensor();
  if (sendCountVal > sendTimeVal) {
    showSensor();
    sendCountVal = 0;
    sCount = String(sendCountVal);
    writeFile(SPIFFS, "/sendCountVal.txt", sCount.c_str());
    send_sim();
    clrRainWind();
  }
  if(serialStatus){
    printf("ESP32: Entering Deep Sleep\n\n");
  } 
  sleep();
}

// ---------------------------------
// ---------------------------- maintenance loop
// ---------------------------------
void maintenance_loop() {
  wdt_reset();
  update_pulse_count();
  updateSensor();
  showSensor();
  delay(3000);
  update_webui();
  dashboard.sendUpdates();
  delay(5000);
  //clrRainWind();
}

// ---------------------------------
// ---------------------------- loop
// ---------------------------------
void loop() {
  if (runStatus) {
    normal_loop();
  } else {
    maintenance_loop();
  }
}

// ---------------------------------
// ---------------------------- ulp
// ---------------------------------
static void init_ulp_program(void) {
  esp_err_t err = ulp_load_binary(0, ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
  ESP_ERROR_CHECK(err);
  assert(rtc_gpio_desc[GPIO_WIND].reg && "GPIO used for pulse counting must be an RTC IO");
  assert(rtc_gpio_desc[GPIO_RAIN].reg && "GPIO used for pulse counting must be an RTC IO");
  ulp_debounce_counter_wind = 2;
  ulp_debounce_counter_rain = 2;
  ulp_debounce_max_count = 3;
  ulp_next_edge_wind = 0;
  ulp_next_edge_rain = 0;
  ulp_io_number_wind = rtc_gpio_desc[GPIO_WIND].rtc_num;
  ulp_io_number_rain = rtc_gpio_desc[GPIO_RAIN].rtc_num;
  pin_setup(GPIO_WIND);
  pin_setup(GPIO_RAIN);
  rtc_gpio_isolate(GPIO_NUM_12);
  rtc_gpio_isolate(GPIO_NUM_15);
  esp_deep_sleep_disable_rom_logging();
  ulp_set_wakeup_period(0, ULP_SLEEP_MS * 1000);
  err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
  ESP_ERROR_CHECK(err);
}

static void update_pulse_count(void) {
  windSpeedVal = (ulp_edge_count_wind & UINT16_MAX) / 2;
  ulp_edge_count_wind = ulp_edge_count_wind % 2;
  rainVal = (ulp_edge_count_rain & UINT16_MAX) / 2;
  ulp_edge_count_rain = ulp_edge_count_rain % 2;
}

static void pin_setup(gpio_num_t gpio_num) {
  rtc_gpio_init(gpio_num);
  rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pulldown_dis(gpio_num);
  rtc_gpio_pullup_dis(gpio_num);
  rtc_gpio_hold_en(gpio_num);
}

static void sleep() {
  delay(100);
  adc_power_off();
  delay(100);
  esp_deep_sleep_start();
}
