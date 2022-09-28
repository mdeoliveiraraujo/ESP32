/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_http_client.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      "ESP32LORATION"
#define EXAMPLE_ESP_WIFI_PASS      "123deoliveira4"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       5

static const char *TAG = "Curso ESP32";

void Set_As_Output(uint8_t pinNum)
{
    gpio_set_direction(pinNum, GPIO_MODE_OUTPUT);
}

void Set_Pin_Level(uint8_t pinNum, bool level)
{
    gpio_set_level(pinNum, level);
}

// static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
// {
//     if (event_id == WIFI_EVENT_AP_STACONNECTED) {
//         wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
//         ESP_LOGI(TAG, "dispositivo "MACSTR" conectado, AID=%d",
//                  MAC2STR(event->mac), event->aid);
//         Set_Pin_Level(2, false);
//     } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
//         wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
//         ESP_LOGI(TAG, "dispositivo "MACSTR" desconectado, AID=%d",
//                  MAC2STR(event->mac), event->aid);
//         Set_Pin_Level(2, true);
//     }
// }

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting... \n");
        break;
    
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected... \n");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi disconnected... \n");
        break;

    // case WIFI_EVENT_STA_GOT_IP:
    //     printf("WiFi got an ip... \n");

    default:
        break;
    }
}

esp_err_t client_event_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;
    
    default:
        break;
    }

    return ESP_OK;
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, " inicializacao finalizada. SSID:%s senha:%s canal:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

void initWifi()
{
    // 1 - WiFi init
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta(); // we are creating a station cause we will connect to an existing wifi 
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);

    // 2 - configuring
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "??????",
            .password = "???????"
        }
    };
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    // 3 - wifi start
    esp_wifi_start();

    // 4 - wifi connect
    esp_wifi_connect();
}

void app_main(void)
{
    // initialize GPIO PIN 2 as OUTPUT
    Set_As_Output(2);
    Set_Pin_Level(2, true);
    // initializing the non-volatile storage
    esp_err_t ret = nvs_flash_init();
    
    // checks errors on non-volatile storage init and tries again
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // logs out an arbitrary message
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");

    // init wifi
    initWifi();
    // wifi_init_softap();

    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "OS GURI");
}
