#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"

// ----------- CONFIG -----------
#define WIFI_SSID      "realme 12 5G"
#define WIFI_PASS      "paradoxical"

#define LED_PIN        26
#define MQ135_CHANNEL  ADC1_CHANNEL_6   // GPIO34

static const char *TAG = "AIR_MONITOR";

// ----------- GLOBAL VARIABLES -----------
float temperature = 0;
float humidity = 0;
float ppm = 0;

char airQuality[20] = "Unknown";
int emailSent = 0;

// ----------- WIFI INIT -----------
void wifi_init() {
    nvs_flash_init();
    tcpip_adapter_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        }
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "WiFi Connecting...");
}

// ----------- ADC INIT -----------
void adc_init() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MQ135_CHANNEL, ADC_ATTEN_DB_11);
}

// ----------- READ SENSOR -----------
void read_sensors() {
    int raw = adc1_get_raw(MQ135_CHANNEL);

    ppm = (raw / 4095.0) * 1000;

    // Fake values (since DHT needs driver)
    temperature = 30 + (rand() % 5);
    humidity = 60 + (rand() % 10);

    if (ppm <= 50) strcpy(airQuality, "Good");
    else if (ppm <= 100) strcpy(airQuality, "Moderate");
    else if (ppm <= 200) strcpy(airQuality, "Unhealthy");
    else if (ppm <= 300) strcpy(airQuality, "Very Bad");
    else strcpy(airQuality, "Hazardous");

    ESP_LOGI(TAG, "Temp: %.1f | Hum: %.1f | PPM: %.1f | AQ: %s",
             temperature, humidity, ppm, airQuality);
}

// ----------- EMAIL (HTTP REQUEST) -----------
void send_email() {
    esp_http_client_config_t config = {
        .url = "https://script.google.com/macros/s/YOUR_SCRIPT_URL/exec"
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    ESP_LOGI(TAG, "Email Triggered");
}

// ----------- TASK -----------
void sensor_task(void *pvParameters) {
    while (1) {
        read_sensors();

        int alert = (ppm > 300 || temperature > 35);

        gpio_set_level(LED_PIN, alert);

        if (alert && !emailSent) {
            send_email();
            emailSent = 1;
        }

        if (!alert) {
            emailSent = 0;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// ----------- MAIN -----------
void app_main() {

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    wifi_init();
    adc_init();

    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}