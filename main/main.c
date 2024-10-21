/**
*************************************************************************
* @file       main.c
* @brief      Source file for the main.c module.
* @details    This file contains the implementation of functions for 
*             the main.c module, including initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       9 de jun. de 2024
* @copyright  Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/

// Standard C Includes
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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
#include "cJSON.h"
#include "main_app.h"
#include "api/wifi_app.h"
#include "api/https_app.h"
#include "api/fw_update.h"

// Tests Includes
#include "main_test.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
                
/**
 * @brief Tag used for ESP serial console messages
 */
static const char TAG [] = "main_app"; 

/**
 * @brief Queue handle used to manipulate the main queue of events
 */
static QueueHandle_t main_app_queue_handle;

/**
 * @brief Structure to hold firmware metadata information
 */
static firmware_metadata_info_t firmware_info = {0};

/**
 * @brief Current state of the main application
 */
main_app_state_e state = MAIN_APP_IDLE;

/**
 * @brief String buffer for the URL used in HTTPS communication
 */
char url_string[URL_LEN] = {0};

/**
 * @brief String buffer for the payload used in HTTPS communication
 */
char payload_string[PAYLOAD_LEN] = {0};


/* Function prototypes ---------------------------------------------------*/

/**
 * @brief Main task for the Main application  
 * @param pvParameters parameter which can be passed to the task
 */
static void main_app_task(void *pvParameters);

/**
 * @brief Process the HTTP response and extract firmware information.
 * @param response The response string from the server.
 * @param len The length of the response string.
 * @param firmware_info Pointer to the firmware metadata information structure.
 */
void main_app_process_response(const char *response, int len, firmware_metadata_info_t *firmware_info);

/**
 * @brief Starts the firmware download.
 */
void main_app_start_firmware_download(void); 


/* Public Functions ------------------------------------------------------*/ 

/**
 * @defgroup main.c Public Functions
 * @{
 */

/**
 * @brief Main application entry point.
 */
void app_main(void){
	
	// Initialize the NVS
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	printf(APP_HEADER);
	printf(APP_VERSION);
	printf(APP_HEADER);
	
	// Select the test to be executed
	main_test_init();
	
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
 * @brief Main task for the application  
 * @param pvParameters parameter which can be passed to the task
 */
static void main_app_task(void *pvParameters){
	main_app_queue_message_t msg;
	
	ESP_LOGI(TAG, "STARTING MAIN APPLICATION");
	
	while(1){
		if(xQueueReceive(main_app_queue_handle, &msg, portMAX_DELAY)){
			switch(msg.msgID){
				case MAIN_APP_MSG_STA_CONNECTED:
				case MAIN_APP_RELOAD:
					ESP_LOGI(TAG, "MAIN_APP_MSG_STA_CONNECTED");	
					
					if(state == MAIN_APP_IDLE){
						state = MAIN_APP_CHECK_FW;
					}
					// Check if there is an update available
					if(state == MAIN_APP_CHECK_FW){
						main_test_update_log("INIT METADATA ACCESS T0");
						strcpy((char*)url_string, ADDRESS_REGISTER_DEVICE);
						strcpy((char*)payload_string, PAYLOAD_REGISTER_DEVICE);
	    				https_app_send_message(HTTPS_APP_MSG_SEND_REQUEST, url_string, payload_string, 0, NULL);
					} 
					if(state == MAIN_APP_UPDATE_STATUS){
						// Inform if the OTA was ok or not
					}
				break;
	 			
	 			case MAIN_APP_MSG_STA_DISCONNECTED:
		 			ESP_LOGI(TAG, "MAIN_APP_MSG_STA_DISCONNECTED");
		 			
		 			// Sends message to the wifi task to connect again
		 			wifi_app_send_message(WIFI_APP_MSG_CONNECTING_STA);
	 			break;
	 			
	 			case MAIN_APP_MSG_HTTPS_CONNECTED:
	 				ESP_LOGI(TAG, "MAIN_APP_MSG_HTTPS_CONNECTED");
	 			break;
	 			
	 			case MAIN_APP_MSG_HTTPS_RECEIVED:
		 			ESP_LOGI(TAG, "MAIN_APP_MSG_HTTPS_RECEIVED");
		 			if(msg.code == HTTPS_RECEIVED_MSG_SUCCESS){
						 ESP_LOGI(TAG, "Message Received: %.*s",msg.len, (char*) msg.data);
						 
						 if(state == MAIN_APP_CHECK_FW){
	    					main_app_process_response((char*) msg.data, msg.len, &firmware_info);
	    					 
	    					 // Log the extracted firmware information
	    					ESP_LOGI("Firmware Info", "Status: %s", firmware_info.status);
				    		if (strcmp(firmware_info.status, VERSION_OUTDATED) == 0) {
								main_test_update_log("RECEIVED METADATA T1 ");
				        		ESP_LOGI("Firmware Info", "Version: %s", firmware_info.version);
				        		ESP_LOGI("Firmware Info", "Author: %s", firmware_info.author);
				        		ESP_LOGI("Firmware Info", "Hardware Model: %s", firmware_info.hardwareModel);
				        		ESP_LOGI("Firmware Info", "Integrity Hash: %s", firmware_info.integrityHash);
				        		ESP_LOGI("Firmware Info", "Timestamp: %s", firmware_info.timestamp);
				        		ESP_LOGI("Firmware Info", "Description: %s", firmware_info.description);
				        		ESP_LOGI("Firmware Info", "CID: %s", firmware_info.cid);
				        		state = MAIN_APP_DOWNLOAD_FW;
				    		}				 
						 }
					 }
					 else{
						ESP_LOGI(TAG,"HTTPS ERROR CODE %d",msg.code);
					}
	 			break;
	 			
	 			case MAIN_APP_MSG_HTTPS_DISCONNECTED:
		 			ESP_LOGI(TAG, "MAIN_APP_MSG_HTTPS_DISCONNECTED");
		 			if(state == MAIN_APP_DOWNLOAD_FW){
						 main_app_start_firmware_download();
						 state = MAIN_APP_DECRYPT_FW;
					 }
					 //main_test_update_loop(); // For Certificate Error test
	 			break;
	 			
	 			case MAIN_APP_FW_DONWLOADED:
	 				ESP_LOGI(TAG, "MAIN_APP_FW_DONWLOADED");
	 				if(state == MAIN_APP_DECRYPT_FW){
						main_test_update_log("INIT FIRMWARE DOWNLOADED T3");
	 					if(decrypt_firmware_from_storage(msg.len) == FW_UPDATE_OK){
							 main_test_update_log("INIT DECRYPT PROCESS T4");
							 if(calculate_sha256_hash_from_ota(firmware_info.integrityHash) == FW_UPDATE_OK){
								main_test_update_log("INIT FIRMWRARE HASH T5");
								ESP_LOGI(TAG, "Initialize Firmware Update");
							 	//apply_firmware_update();
							 	main_test_update_loop();
							 	
							 }
							 else
							 	main_test_update_loop(); // For HASH Error test
						 }
						 else
						 	main_test_update_loop(); // For Decrypt Error test
	 				}
	 				state = MAIN_APP_IDLE;
	 			break;
	 			
	 			default:
                	ESP_LOGI(TAG, "Unknown message ID");
                break;
			}
			
			// Release memory allocated for the strings
			if(msg.data){
				free((void*)msg.data);
			}			
		}
	}
}

/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the wifi_app_message_e enum
 * @param code Code associated with the message
 * @param len Length of the data
 * @param data Pointer to the data
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t main_app_send_message(main_app_message_e msgID, int code, int len, const char* data){
	static main_app_queue_message_t msg;
	msg.msgID = msgID;
	msg.code = code;
	msg.len = len;
	msg.data = NULL;
	
	if(data) {
         msg.data = strdup(data);
         if (msg.data == NULL) {
             ESP_LOGI(TAG, "Failed to allocate memory for Data");
             return pdFALSE;
         }
    }
	BaseType_t result = xQueueSend(main_app_queue_handle, &msg, portMAX_DELAY);
	if (result != pdTRUE){
		if(msg.data){
			free((void*)msg.data);
		}
	}
	return result;
}
	
/**
 * @brief Process the HTTP response and extract firmware information.
 * @param response The response string from the server.
 * @param len The length of the response string.
 * @param firmware_info Pointer to the firmware metadata information structure.
 */
void main_app_process_response(const char *response, int len, firmware_metadata_info_t *firmware_info){
    
    // Check if the response is a known plain text message
    if (strstr(response, ERROR_HW_NOT_FOUND) || strstr(response, VERSION_UPDATED)) {
        strncpy(firmware_info->status, response, len);
        firmware_info->status[sizeof(firmware_info->status) - 1] = '\0';  // Ensure null termination
        return;
    }

    // Try to parse the response as JSON
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        ESP_LOGE("JSON", "Error parsing JSON");
        return;
    }

    // Extract the "message" field
    cJSON *message = cJSON_GetObjectItem(json, "message");
    if (cJSON_IsString(message) && (message->valuestring != NULL)) {
        strncpy(firmware_info->status, message->valuestring, sizeof(firmware_info->status) - 1);
        firmware_info->status[sizeof(firmware_info->status) - 1] = '\0';  // Ensure null termination

        if (strcmp(firmware_info->status, VERSION_OUTDATED) == 0) {
            // Extract the "latestFirmware" object
            cJSON *latestFirmware = cJSON_GetObjectItem(json, "latestFirmware");
            if (!cJSON_IsObject(latestFirmware)) {
                ESP_LOGE("JSON", "Error: latestFirmware is not an object");
                cJSON_Delete(json);
                return;
            }

            // Extract the individual fields from the "latestFirmware" object and store them in the struct
            cJSON *version = cJSON_GetObjectItem(latestFirmware, "version");
            cJSON *author = cJSON_GetObjectItem(latestFirmware, "author");
            cJSON *hardwareModel = cJSON_GetObjectItem(latestFirmware, "hardwareModel");
            cJSON *integrityHash = cJSON_GetObjectItem(latestFirmware, "integrityHash");
            cJSON *timestamp = cJSON_GetObjectItem(latestFirmware, "timestamp");
            cJSON *description = cJSON_GetObjectItem(latestFirmware, "description");
            cJSON *cid = cJSON_GetObjectItem(latestFirmware, "cid");

            if (cJSON_IsString(version) && (version->valuestring != NULL)) {
                strncpy(firmware_info->version, version->valuestring, sizeof(firmware_info->version) - 1);
            }
            if (cJSON_IsString(author) && (author->valuestring != NULL)) {
                strncpy(firmware_info->author, author->valuestring, sizeof(firmware_info->author) - 1);
            }
            if (cJSON_IsString(hardwareModel) && (hardwareModel->valuestring != NULL)) {
                strncpy(firmware_info->hardwareModel, hardwareModel->valuestring, sizeof(firmware_info->hardwareModel) - 1);
            }
            if (cJSON_IsString(integrityHash) && (integrityHash->valuestring != NULL)) {
                strncpy(firmware_info->integrityHash, integrityHash->valuestring, sizeof(firmware_info->integrityHash) - 1);
            }
            if (cJSON_IsString(timestamp) && (timestamp->valuestring != NULL)) {
                strncpy(firmware_info->timestamp, timestamp->valuestring, sizeof(firmware_info->timestamp) - 1);
            }
            if (cJSON_IsString(description) && (description->valuestring != NULL)) {
                strncpy(firmware_info->description, description->valuestring, sizeof(firmware_info->description) - 1);
            }
            if (cJSON_IsString(cid) && (cid->valuestring != NULL)) {
                strncpy(firmware_info->cid, cid->valuestring, sizeof(firmware_info->cid) - 1);
            }	
        }
    }

    // Free the JSON object
    cJSON_Delete(json);
}

/**
 * @brief Starts the firmware download.
 */
void main_app_start_firmware_download(void){
	main_test_update_log("INIT FIRMWARE IPFS DOWNLOAD T2");
	strcpy((char*)url_string, HTTPS_IPFS_SERVER_URL);
	//strcat((char*)url_string, "QmeYizCjAByRsLYvqGXwP3Vu1mpUyipGD8DMV6DZedfTtP"); // Curto 128
	//strcat((char*)url_string, "QmW3a4Vu3zkyeLAkMnUGe6ejkCXTmeTnhs9jQ5iTT584hE"); // Longo 128
	//strcat((char*)url_string, "QmUzRXXm4VgHPc42NkJYo8fzQ4Dv6pRFkPkpfZugbFwhL4"); // Curto 256
	strcat((char*)url_string, "QmYmXS2FE72kciXwf9qCVtgNvrH1nsx2aua4cGu1kSDNH8"); //longo 256
	//strcat((char*)url_string, firmware_info.cid);
	ESP_LOGI(TAG, "Firmware url: %s",url_string);
	https_app_send_message(HTTPS_APP_MSG_DOWNLOAD_FW, url_string, NULL, 0, NULL);
}

/** @} */
