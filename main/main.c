/**
*************************************************************************
* @file			main.c
* @brief		Source file for the main.c module.
* @details		This file contains the implementation of functions for 
*				the main.c module, including initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			9 de jun. de 2024
* @version		1.0.0
* @note			Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/

// Standard C Includes
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Include Systems configuration
#include "sysconfig.h"

// FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

// ESP Includes
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_interface.h"
#include "esp_err.h"
#include "esp_log.h"

// Application Includes
#include "portmacro.h"
#include "tasks_common.h"
#include "main_app.h"
#include "api/wifi_app.h"
#include "api/https_app.h"
/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/

const char test_url[] = "https://18.230.239.105:3000/register-device";
const char test_payload[] = "{\"hardwareVersion\": \"ModelX\", \"softwareVersion\": \"v1.1\"}";
    			
// Tag used for ESP serial console msgs
static const char TAG [] = "main_app"; 

// Queue handle used to manipulate the main queue of events
static QueueHandle_t main_app_queue_handle;

/* Function prototypes ---------------------------------------------------*/

/**
 * @brief  Main task for the Main application  
 * @param  pvParameters parameter which can be passed to the task
 */
static void main_app_task(void *pvParameters);

/* Public Functions ------------------------------------------------------*/ 

/**
 * @defgroup main.c Public Functions
 * @{
 */
 
void app_main(void){
	
	// Initialize the NVS
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

    // Create message queue
    main_app_queue_handle = xQueueCreate(3, sizeof(main_app_queue_message_t));
	
	// Start WiFi
	wifi_app_start();
	
	// Start HTTPS
	https_app_start();
	
	// Start the Main application task
    xTaskCreate(&main_app_task, "main_app_task", MAIN_APP_TASK_STACK_SIZE, NULL, MAIN_APP_TASK_PRIORITY, NULL);
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup main.c Private Functions
 * @{
 */
 
  /**
 * @brief  Main task for the  application  
 * @param  pvParameters parameter which can be passed to the task
 */
static void main_app_task(void *pvParameters){
	main_app_queue_message_t msg;
	
	ESP_LOGI(TAG, "STARTING MAIN APPLICATION");
	
	while(1){
		if(xQueueReceive(main_app_queue_handle, &msg, portMAX_DELAY)){
			switch(msg.msgID){
				case MAIN_APP_MSG_STA_CONNECTED:
				ESP_LOGI(TAG, "MAIN_APP_MSG_STA_CONNECTED");	
				
				// Sends message to the https task
    			https_app_send_message(HTTPS_APP_MSG_SEND_REQUEST, test_url, test_payload, 0, NULL);

				break;
	 			
	 			case MAIN_APP_MSG_STA_DISCONNECTED:
	 			ESP_LOGI(TAG, "MAIN_APP_MSG_STA_DISCONNECTED");
	 			break;
	 			
	 			default:
                ESP_LOGI(TAG, "Unknown message ID");
                break;
			}
		}
	}
}

/**
 * @brief Sends a message to the queue
 * @param msgID messae ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFalse
 * @note Expand the parameter list based on your requirements e.g. how you've expanded the wifi_app_queue_message_t. 
 */
BaseType_t main_app_send_message(main_app_message_e msgID){
	main_app_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(main_app_queue_handle, &msg, portMAX_DELAY);
}
 /** @} */