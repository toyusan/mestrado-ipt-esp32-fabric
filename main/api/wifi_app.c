/**
*************************************************************************
* @file       wifi_app.c
* @brief      Source file for the wifi_app.c module.
* @details    This file contains the implementation of functions for 
*             the wifi_app.c module, including initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       9 de jun. de 2024
* @note       Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/
#include "sysconfig.h"

// FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

// ESP Includes
#include "esp_event.h"
#include "esp_interface.h"
#include "esp_netif_types.h"
#include "esp_wifi_default.h"
#include "esp_wifi_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

// Application Includes
#include "portmacro.h"
#include "tasks_common.h"
#include "main_app.h"
#include "api/wifi_app.h"
#include "api/https_app.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/

// Tag used for ESP serial console messages
static const char TAG [] = "wifi_app"; 

// Used for returning the WiFi configuration
wifi_config_t *wifi_config = NULL;

// Used to track the number for retries when a connection attempt fails
static int g_retry_number;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t wifi_app_queue_handle;

// netif objects for the station
esp_netif_t* esp_netif_sta = NULL;

/* Function prototypes ---------------------------------------------------*/

/**
 * @brief Main task for the WiFi application  
 * @param pvParameters parameter which can be passed to the task
 */
static void wifi_app_task(void *pvParameters);

/**
 * @brief Initializes the TCP stack and default WiFi configuration
 */
static void wifi_app_default_wifi_init(void);

/**
 * @brief Initializes the WiFi application event handler for the WiFi and IP events
 */
static void wifi_app_event_handler_init(void);

/**
 * @brief WiFi application event handler
 * @param arg_data aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id from the event to register the handler for
 * @param event_data event data
 */
static void wifi_app_event_handler(void* arg_data, esp_event_base_t event_base, int32_t event_id, void* event_data);

/**
 * @brief Initializes the WiFi sta settings.
 */
static void wifi_app_soft_sta_config(void);

/**
 * @brief Connects the ESP32 to an external AP using the updated station configuration
 */
static void wifi_app_connect_sta(void);

/* Public Functions ------------------------------------------------------*/ 
/**
 * @defgroup wifi_app.c Public Functions
 * @{
 */
 
/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID){
	wifi_app_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

/**
 * @brief Starts the WiFi RTOS task
 */
void wifi_app_start (void){
	ESP_LOGI(TAG, "STARTING WIFI APPLICATION");
	 
	// Start WiFi started LED
	 
	// Disable default WiFi logging message
	esp_log_level_set("wifi", ESP_LOG_NONE);

	// Allocate memory for the wifi configuration
	wifi_config = (wifi_config_t*)malloc(sizeof(wifi_config_t));
	memset(wifi_config, 0x00, sizeof(wifi_config_t));
		 
	// Create message queue
	wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));
	 
	// Start the WiFi application task
	xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_TASK_CORE_ID);
}

/**
 * @brief Initializes the WiFi sta settings.
 */
static void wifi_app_soft_sta_config(void){
	char *ssid_str = NULL, *pass_str = NULL;
	size_t len_ssid = 0, len_pass = 0;
	
	// Get SSID header
	len_ssid = strlen(PERSONAL_SSID) + 1;
	if (len_ssid > 1){
		ssid_str = malloc(len_ssid);
		strcpy(ssid_str, PERSONAL_SSID);
		//printf("ssid_str %s len_ssid %d\n", ssid_str, len_ssid);
	}
	
	// Get Password header
	len_pass = strlen(PERSONAL_PASS) + 1;
	if (len_pass > 1){
		pass_str = malloc(len_pass);
		strcpy(pass_str, PERSONAL_PASS);
		//printf("pass_str %s len_pass %d\n", pass_str, len_pass);
	}
	
    // Update the Wifi networks configuration and let the wifi application know
	wifi_config_t* wifi_config = wifi_app_get_wifi_config();
	memset(wifi_config, 0x00, sizeof(wifi_config_t));
	memcpy(wifi_config->sta.ssid, ssid_str, len_ssid);
	memcpy(wifi_config->sta.password, pass_str, len_pass);
	//printf("Connect to  %s - %s\n", wifi_config->sta.ssid, wifi_config->sta.password);
	ESP_LOGI(TAG, "Connect to  %s - %s", wifi_config->sta.ssid, wifi_config->sta.password);
}

/**
 * @brief Connects the ESP32 to an external AP using the updated station configuration
 */
static void wifi_app_connect_sta(void){
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_app_get_wifi_config()));
	ESP_ERROR_CHECK(esp_wifi_connect());
}

/**
 * @brief Gets the wifi configuration
 * @return Pointer to the WiFi configuration
 */
wifi_config_t* wifi_app_get_wifi_config(void){
	return wifi_config;
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup wifi_app.c Private Functions
 * @{
 */

/**
 * @brief Main task for the WiFi application  
 * @param pvParameters parameter which can be passed to the task
 */
static void wifi_app_task(void *pvParameters){
	wifi_app_queue_message_t msg;
	
	// Initialize the event handler
	wifi_app_event_handler_init();
	
	// Initialize the TCP/IP stack and WiFi configuration
	wifi_app_default_wifi_init();
	
	// Start WiFi
	ESP_ERROR_CHECK(esp_wifi_start());
	
	// Configure the WiFi Network
	wifi_app_soft_sta_config();
	
	// Connect to the WiFi Network
	wifi_app_send_message(WIFI_APP_MSG_CONNECTING_STA);
	
	while(1){
		if(xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY)){
			switch(msg.msgID){
				case WIFI_APP_MSG_CONNECTING_STA:
				ESP_LOGI(TAG, "WIFI_APP_MSG_CONNECTING_STA");
				// Attempt a connection
				wifi_app_connect_sta();
				// Set current number of retries to zero
				g_retry_number = 0;
				break;
				
				case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
				ESP_LOGI(TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
				// Notify that is connected
				main_app_send_message(MAIN_APP_MSG_STA_CONNECTED, 0 ,0, NULL);
				break;

				case WIFI_APP_MSG_STA_DISCONNECTED:
				ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED");
				// Notify that is disconnected
				main_app_send_message(MAIN_APP_MSG_STA_DISCONNECTED, 0,0, NULL);
				break;			
				
				default:
				break;
			}
		}
	}
}

/**
 * @brief Initializes the WiFi application event handler for the WiFi and IP events
 */
static void wifi_app_event_handler_init(void){
	
	// Event loop for the WiFi driver
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	
	// Event handler for the connection
	esp_event_handler_instance_t instance_wifi_event;
	esp_event_handler_instance_t instance_ip_event;
	
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_wifi_event));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_ip_event));
}

/**
 * @brief WiFi application event handler
 * @param arg_data aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id from the event to register the handler for
 * @param event_data event data
 */
static void wifi_app_event_handler(void* arg_data, esp_event_base_t event_base, int32_t event_id, void* event_data){
	if(event_base == WIFI_EVENT){
		switch(event_id){
			case WIFI_EVENT_AP_START:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
			break;
			
			case WIFI_EVENT_AP_STOP:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
			break;
			
			case WIFI_EVENT_AP_STACONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
			break;
			
			case WIFI_EVENT_AP_STADISCONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
			break;
			
			case WIFI_EVENT_STA_START:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
			break;
			
			case WIFI_EVENT_STA_CONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
			break;
			
			case WIFI_EVENT_STA_DISCONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
			wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = (wifi_event_sta_disconnected_t*)malloc(sizeof(wifi_event_sta_disconnected_t));
			*wifi_event_sta_disconnected = *((wifi_event_sta_disconnected_t*)event_data);
			//printf("WIFI_EVENT_STA_DISCONNECTED, reason code %d\n", wifi_event_sta_disconnected->reason);
			ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED, reason code %d\n", wifi_event_sta_disconnected->reason);
			if (g_retry_number < MAX_CONNECTION_RETRIES){
				esp_wifi_connect();
				g_retry_number ++;
			}
			else{
				wifi_app_send_message(WIFI_APP_MSG_STA_DISCONNECTED);
			}
			break;
		}
	}
	else if(event_base == IP_EVENT){
		switch(event_id){
			case IP_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG,"IP_EVENT_STA_GOT_IP");
			wifi_app_send_message(WIFI_APP_MSG_STA_CONNECTED_GOT_IP);
			break;
		}
	}
}

/**
 * @brief Initializes the TCP stack and default WiFi configuration
 */
static void wifi_app_default_wifi_init(void){
	
	// Initialize the TCP stack
	ESP_ERROR_CHECK(esp_netif_init());
	
	// Default WiFi configuration - operations must be in this order!
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_netif_sta = esp_netif_create_default_wifi_sta();
}

/** @} */
