# Air Monitoring System
ESP32 IoT Air Quality Monitor: A real-time air quality monitoring system built with ESP-IDF for the ESP32. This project measures gas concentration (PPM) using an MQ135 sensor, provides local LED feedback, and sends email alerts via a Google Apps Script webhook when air quality reaches hazardous levels.
Features
ADC Data Acquisition: Reads raw analog signals from the MQ135 sensor.
Multi-Level Logic: Categorizes air quality into 5 distinct levels (Good to Hazardous).
Dual Alert System: * Local: Red LED triggers on GPIO26.
Remote: Automated email notification via HTTP GET request.
Robust Task Management: Built on FreeRTOS to ensure the WiFi stack and sensor polling don't interfere.
Logging: Detailed ESP_LOG output for real-time debugging.

Hardware Components
Microcontroller: ESP32 (WROOM-32)
Sensor: MQ135 Gas Sensor
Indicator: 5mm LED + 220Ω Resistor
Power: USB or 5V External Supply

⚙️ Software Setup
1. Prerequisites
ESP-IDF Framework (v4.4 or later recommended).

A Google Account (for the Email Webhook).

2. Configure WiFi & Webhook
Open main.c and update the following lines:

Configure WiFi & Webhook
Open main.c and update the following lines:

C
#define WIFI_SSID "Your_Network_Name"
#define WIFI_PASS "Your_Password"

// Inside send_email()
.url = "https://script.google.com/macros/s/YOUR_DEPLOYED_URL/exec"

. Build & Flash
Connect your ESP32 to your PC and run:

Bash
# Set the target
idf.py set-target esp32

# Build, Flash, and Monitor
idf.py build flash monitor

 Air Quality ThresholdsThe system maps the 12-bit ADC value ($0$ to $4095$) to a PPM scale:PPM ValueAir Quality StatusLED StateEmail
 Alert0 - 50GoodOFFNo51 - 100ModerateOFFNo101 - 200UnhealthyOFFNo201 - 300Very BadOFFNo> 300HazardousONYES.

 ⚠️ Important Notes
Sensor Warm-up: The MQ135 requires a "burn-in" period. It may provide inconsistent readings for the first 24 hours of operation.
HTTPS/SSL: This code uses esp_http_client for a simple GET request. For production, ensure you handle SSL certificates if your webhook requires strict HTTPS validation.
Simulated Data: Currently, Temperature and Humidity are simulated values. To use a physical DHT sensor, you will need to add a DHT-ESP-IDF component.
🤝 Contributing
Contributions are welcome! If you have ideas for adding a real DHT11 driver or a web dashboard, feel free to fork this repo and submit a pull request.
