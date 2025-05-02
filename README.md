# 🌧️ ESP32-AWLR – Automatic Weather & Level Recorder

**ESP32-AWLR** is a low-power, dual-mode environmental monitoring system built on ESP32. It logs weather parameters (temperature, humidity, pressure, wind speed, rainfall) and water levels using ultrasonic sensors. The device supports both **normal (data logging)** and **maintenance (web UI)** modes, and transmits data via **SIM800L GSM module**.

---

## 🔧 Key Features

- 💤 **Deep Sleep & ULP Coprocessor**: Wind & rain counted while ESP32 sleeps
- 🌡️ Sensor readings: temperature, humidity, pressure (HDC1080, BMP280)
- 🌧️ Rainfall and wind speed detection via pulse inputs (GPIO 34 & 35)
- 📶 SIM800L-based GSM communication
- 📆 DS3231 RTC for timestamping
- 💾 SPIFFS for persistent parameter storage
- 🌐 Wi-Fi dashboard via Access Point (maintenance mode)
- 📊 [ESPDash](https://github.com/ayushsharma82/ESPDash) for intuitive web interface

---

## 📁 Project Structure

```

esp32-awlr/
├── LICENSE                         # Project license
├── project
│   ├── data
│   │   ├── apiVal.txt              # API key or ID for server upload
│   │   ├── batCalibVal.txt         # Calibration constant for battery
│   │   ├── fbatCalibVal.txt        # Calculated float battery value
│   │   ├── rainTotalVal.txt        # Cumulative rainfall value
│   │   ├── sendCountVal.txt        # Current count toward next send
│   │   ├── sendTimeVals.txt        # Interval between SIM sends
│   │   ├── sendTimeVal.txt         # Interval between SIM sends
│   │   ├── serialStatus.txt        # Enable/disable serial debug
│   │   ├── simStatus.txt           # Flag to trigger SIM send
│   │   ├── ultrasonicGroundAVal.txt    # Calibration offset for ultrasonic A
│   │   ├── ultrasonicGroundBVal.txt    # Calibration offset for ultrasonic B           
│   │   └── windTotalVal.txt        # Cumulative wind value
│   ├── datetime.ino                # RTC-related functions
│   ├── esp32-awlr-template.ino     # Main firmware logic (setup & mode control)
│   ├── http.ino                    # HTTP server & handlers
│   ├── sd.ino                      # SD card (optional) interface  
│   ├── sensor.ino                  # Sensor reading logic
│   ├── ulp_main.h                  # ULP pulse counting logic
│   ├── ulp.s                       # ULP assembly code
│   ├── variable.h                  # Global variables & settings
│   ├── wdt.ino                     # Watchdog functions
│   └── webUI.ino                   # Web dashboard (maintenance mode)
└── README.md                       # Project documentation

```

---

## 🛠️ Hardware Requirements

| Component              | Description                               |
|------------------------|-------------------------------------------|
| ESP32 Dev Module       | Main microcontroller                      |
| SIM800L Module         | For SMS/data communication                |
| HDC1080 Sensor         | For humidity and temperature              |
| BMP280 Sensor          | For pressure and temperature              |
| DS3231 RTC             | Real-time clock with battery backup       |
| Wind & Rain Sensors    | Pulse-output based sensors                |
| Ultrasonic Sensor x2   | For measuring water level (A & B)         |
| Buzzer & Switches      | Manual control and alerts                 |
| MicroSD (optional)     | For data logging (via `sd.ino`)           |

---

### 🔌 Wiring Table

| Component                      | ESP32 Pin     | Description                                   |
| ------------------------------ | ------------- | --------------------------------------------- |
| **SIM800L TX**                 | GPIO 17       | ESP32 RX (receives from SIM800L)              |
| **SIM800L RX**                 | GPIO 16       | ESP32 TX (sends to SIM800L)                   |
| **SIM800L RESET**              | GPIO 15       | Reset control for SIM800L                     |
| **Ultrasonic Sensor A Trig**   | GPIO 26       | Trigger pin for ultrasonic sensor A           |
| **Ultrasonic Sensor A Echo**   | GPIO 33       | Echo pin for ultrasonic sensor A              |
| **Ultrasonic Sensor B Trig**   | GPIO 14       | Trigger pin for ultrasonic sensor B           |
| **Ultrasonic Sensor B Echo**   | GPIO 13       | Echo pin for ultrasonic sensor B              |
| **Wind Sensor (Pulse)**        | GPIO 35 (RTC) | Pulse-counted by ULP coprocessor              |
| **Rain Sensor (Pulse)**        | GPIO 34 (RTC) | Pulse-counted by ULP coprocessor              |
| **Switch 1 (SW1)**             | GPIO 12       | Generic input switch                          |
| **Switch 2 (SW2)**             | GPIO 4        | Mode select: Normal / Maintenance             |
| **Switch 3 (SW3)**             | GPIO 2        | Generic input switch                          |
| **Alarm Pin**                  | GPIO 27       | Digital input, used for alarm signal          |
| **Interrupt Pin (INT)**        | GPIO 27       | Possibly shared with alarmPin (same pin used) |
| **Buzzer**                     | GPIO 25       | Active LOW control output for buzzer          |
| **WDT Trigger (Keep Alive)**   | GPIO 32       | Watchdog refresh pin                          |
| **SD Card CS (if used)**       | GPIO 5        | Chip select for SPI SD card                   |
| **I2C SDA** (RTC, BMP280, etc) | GPIO 21 (def) | I2C data line                                 |
| **I2C SCL** (RTC, BMP280, etc) | GPIO 22 (def) | I2C clock line                                |

> ⚠️ Make sure all I2C components (HDC1080, BMP280, ADS1115, RTC) share **SDA (GPIO 21)** and **SCL (GPIO 22)**.
> ⚠️ Use level shifters or logic conversions if needed, especially when using SIM800L and 5V sensors.

---
## ⚡ Operating Modes

### 1. **Normal Mode** (Switch `SW2` = HIGH)
- Device sleeps and wakes periodically (default: 30 minutes)
- Collects & sends data via SIM800L
- Wind & rain counts are recorded using ULP even during sleep

### 2. **Maintenance Mode** (Switch `SW2` = LOW)
- Starts Wi-Fi Access Point
- Provides web dashboard via ESPDash
- Allows real-time monitoring without sleep

---

## 🌐 Wi-Fi Access (Maintenance Mode)

```cpp
WiFi.softAP(ssid, password);
```

* **SSID**: `ESP32-AWLR`
* **Password**: *(defined in `variable.h`)*
* Access dashboard at `http://192.168.4.1/` after connecting

---

## 🧠 ULP Pulse Counter

* Wind sensor on **GPIO 35**
* Rain sensor on **GPIO 34**
* ULP co-processor monitors pulses while main core sleeps
* `ulp.s` and `ulp_main.h` implement low-power logic

---

## 💾 SPIFFS Stored Parameters

| File Name                  | Purpose                             |
| -------------------------- | ----------------------------------- |
| `apiVal.txt`               | API key or ID for server upload     |
| `batCalibVal.txt`          | Calibration constant for battery    |
| `fbatCalibVal.txt`         | Calculated float battery value      |
| `sendTimeVal.txt`          | Interval between SIM sends          |
| `sendCountVal.txt`         | Current count toward next send      |
| `serialStatus.txt`         | Enable/disable serial debug         |
| `simStatus.txt`            | Flag to trigger SIM send            |
| `ultrasonicGroundAVal.txt` | Calibration offset for ultrasonic A |
| `ultrasonicGroundBVal.txt` | Calibration offset for ultrasonic B |
| `rainTotalVal.txt`         | Cumulative rainfall value           |
| `windTotalVal.txt`         | Cumulative wind value               |

---

## 🔁 Loop Behavior

| Loop                 | Description                                       |
| -------------------- | ------------------------------------------------- |
| `normal_loop()`      | Sleep, wake, count pulses, send data on interval  |
| `maintenance_loop()` | Web server, live dashboard updates                |
| `loop()`             | Chooses between normal/maintenance based on `SW2` |

---

## 🧪 Serial Monitor Output Example

```txt
Running Mode: Normal
Read HDC1080: Temp=28.32C Hum=76.20%
Read BMP280: Temp=27.91C Pressure=1008.42 hPa
Wind Pulse: 8 -> Wind Speed: 4 m/s
Rain Pulse: 5 -> Rainfall: 2.5 mm
ESP32: Entering Deep Sleep
```

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 🙋‍♂️ Author

Developed by **Ardy Seto**
For real-world environmental monitoring research and IoT deployments.