/**
*************************************************************************
* @file       https_app.c
* @brief      Source file for the https_app.c module.
* @details    This file contains the implementation of functions for 
*             the https_app.c module, including initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       15 de jun. de 2024
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
#include "esp_err.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"

// Application Includes
#include "portmacro.h"
#include "tasks_common.h"
#include "main_app.h"
#include "api/https_app.h"
#include "api/fw_update.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
/**
 * @brief Tag used for ESP serial console messages
 */
static const char TAG [] = "https_app";

/**
 * @brief Queue handle used to manipulate the main queue of events
 */
static QueueHandle_t https_app_queue_handle;

/**
 * @brief Buffer to store the response from the HTTPS request
 */
static char g_response_buffer[HTTPS_RESPONSE_BUFFER_SIZE];

/**
 * @brief Buffer to store the response to be sent
 */
static char g_response_buffer_to_send[HTTPS_RESPONSE_BUFFER_SIZE];

/**
 * @brief Length of the response
 */
static int g_len = 0;

/**
 * @brief Flag to indicate firmware download
 */
static int g_fw_flag = 0;

/* Function prototypes ---------------------------------------------------*/

/**
 * @brief Main task for the HTTPS application
 * @param pvParameters parameter which can be passed to the task
 */
static void https_app_task(void *pvParameters);

/**
 * @brief Internal function to perform the HTTPS request
 * @param url URL to send the request
 * @param payload Data to send
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
static esp_err_t https_app_perform_request(const char *url, const char *payload);

/**
 * @brief Internal function to download firmware
 * @param url URL to download the firmware
 */
static void http_app_download_firmware(const char *url);

/* Public Functions ------------------------------------------------------*/

/**
 * @defgroup https_app.c Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the https_app_message_e enum
 * @param url URL to send the request
 * @param payload Data to send
 * @param response_code Response code from the server
 * @param response_message Response message from the server
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t https_app_send_message(https_app_message_e msgID, const char *url, const char *payload, int response_code, const char* response_message) {
    static https_app_queue_message_t msg;
    msg.msgID = msgID;
    msg.url = NULL;
    msg.payload = NULL;
    msg.response_message = NULL;
    
    if(url) {
         msg.url = strdup(url);
         if (msg.url == NULL) {
             ESP_LOGI(TAG, "Failed to allocate memory for URL");
             return pdFALSE;
         }
    }
    if(payload) {
         msg.payload = strdup(payload);
         if (msg.payload == NULL) {
             ESP_LOGI(TAG, "Failed to allocate memory for payload");
             free((void*)msg.url); // Cleanup previously allocated memory
             return pdFALSE;
         }
    }
    msg.response_code = response_code;
    if(response_message) {
        msg.response_message = strdup(response_message);
        if (msg.response_message == NULL) {
            ESP_LOGI(TAG, "Failed to allocate memory for response_message");
            free((void*)msg.url);      // Cleanup previously allocated memory
            free((void*)msg.payload);  // Cleanup previously allocated memory
            return pdFALSE;
        }
    }
    BaseType_t result = xQueueSend(https_app_queue_handle, &msg, portMAX_DELAY);
    //printf("result code %d\n", result);
    // Release memory if it fails
    if (result != pdTRUE) {
        if (msg.url) {
            free((void*)msg.url);
        }
        if (msg.payload) {
            free((void*)msg.payload);
        }
        if (msg.response_message) {
            free((void*)msg.response_message);
        }
    }
    return result;
}

/**
 * @brief Starts the HTTPS RTOS task
 */
void https_app_start(void) {
    ESP_LOGI(TAG, "STARTING HTTPS APPLICATION");

    // Create message queue
    https_app_queue_handle = xQueueCreate(3, sizeof(https_app_queue_message_t));
	if (https_app_queue_handle == NULL) {
   		 ESP_LOGI(TAG, "Failed to create queue");
	}
	esp_log_level_set("esp-tls", ESP_LOG_DEBUG);
	esp_log_level_set("esp-tls-mbedtls", ESP_LOG_DEBUG);

    // Start the HTTPS application task
    xTaskCreate(&https_app_task, "https_app_task", HTTPS_APP_TASK_STACK_SIZE, NULL, HTTPS_APP_TASK_PRIORITY, NULL);
}
/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup https_app.c Private Functions
 * @{
 */
 
/**
 * @brief Main task for the HTTPS application
 * @param pvParameters parameter which can be passed to the task
 */
static void https_app_task(void *pvParameters) {
    https_app_queue_message_t msg;

    while (1) {
        if (xQueueReceive(https_app_queue_handle, &msg, portMAX_DELAY)) {
            switch (msg.msgID) {
				case HTTPS_APP_MSG_SEND_REQUEST:
                    ESP_LOGI(TAG, "HTTPS_APP_MSG_SEND_REQUEST");
                    https_app_perform_request(msg.url, msg.payload);
                    break;
                    
                case HTTPS_APP_MSG_DOWNLOAD_FW:
                    ESP_LOGI(TAG, "HTTPS_APP_MSG_DOWNLOAD_FW");
                    http_app_download_firmware(msg.url);
                    break;
                           
                default:
                    ESP_LOGI(TAG, "Unknown message ID");
                    break;
            }
            
            // Release memory allocated for the strings
            if (msg.url) {
                free((void*)msg.url);
            }
            if (msg.payload) {
                free((void*)msg.payload);
            }
            if (msg.response_message) {
                free((void*)msg.response_message);
            }
        }
    }
}

/**
 * @brief Event handler for HTTP client events
 * @param evt Pointer to HTTP client event structure
 * @return ESP_OK on success, or an error code on failure
 */
esp_err_t client_event_handler(esp_http_client_event_t *evt) {
	
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            main_app_send_message(MAIN_APP_MSG_HTTPS_CONNECTED, 0,0, NULL);
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
           // ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if(!g_fw_flag){
            	if(g_len + evt->data_len < HTTPS_RESPONSE_BUFFER_SIZE){
					strncat(g_response_buffer_to_send, (char*)evt->data, evt->data_len);
					g_len += evt->data_len;
            	}
            	else{
					ESP_LOGI(TAG,"RESPONSE BUFFER OVERFLOW");
				}
			}
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if(!g_fw_flag){
            	main_app_send_message(MAIN_APP_MSG_HTTPS_RECEIVED, HTTPS_RECEIVED_MSG_SUCCESS, g_len, g_response_buffer_to_send);
            	memset(g_response_buffer_to_send, 0x00, HTTPS_RESPONSE_BUFFER_SIZE);
            	g_len = 0;
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            main_app_send_message(MAIN_APP_MSG_HTTPS_DISCONNECTED, 0, 0,NULL);
            break;
  		case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
       default:
       break;
    }
    return ESP_OK;
}

/**
 * @brief Internal function to perform the HTTPS request
 * @param url URL to send the request
 * @param payload Data to send
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
static esp_err_t https_app_perform_request(const char *url, const char *payload) {
    // HTTPS configuration
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)ca_cert_pem_start,
        .cert_len = ca_cert_pem_end - ca_cert_pem_start,
        .client_cert_pem = (const char *)client_cert_pem_start,
        .client_cert_len = client_cert_pem_end - client_cert_pem_start,
        .client_key_pem = (const char *)client_key_pem_start,
        .client_key_len = client_key_pem_end - client_key_pem_start,
        .event_handler = client_event_handler,
        .skip_cert_common_name_check = true, // Ignorar a verificação do nome comum do certificado
        .use_global_ca_store = false,        // Opcionalmente, desabilitar o uso da loja de CA global
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };

    // Initialize HTTPS client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Payload requisition configuration
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Send requisition
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        // Get answer
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        esp_http_client_read(client, g_response_buffer, sizeof(g_response_buffer));

        ESP_LOGI(TAG, "HTTPS POST Status = %d, content_length = %d", status_code, content_length);
        if(content_length > 0)
        	ESP_LOGI(TAG, "Response: %s", g_response_buffer);
    } else {
        ESP_LOGE(TAG, "HTTPS POST request failed: %s", esp_err_to_name(err));
    }

    // Cleanup HTTPS
    esp_http_client_cleanup(client);

    return err;
}

/**
 * @brief Internal function to download firmware
 * @param url URL to download the firmware
 */
static void http_app_download_firmware(const char *url){
	g_fw_flag = 1;
	
   esp_http_client_config_t config = {
        .url = url,
        .event_handler = client_event_handler,
    };

    ESP_LOGI(TAG, "INITIALIZE FIRMWARE DOWNLOAD");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP connection");
        g_fw_flag = 0;
        return;
    }
    ESP_LOGI(TAG, "HTTP CONNECTED");
    
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        g_fw_flag = 0;
        return;
    }
    ESP_LOGI(TAG, "HTTP CLIENT OPENED");
    
    int content_length = esp_http_client_fetch_headers(client);
    if (content_length < 0) {
        ESP_LOGE(TAG, "HTTP client fetch headers failed");
        esp_http_client_cleanup(client);
        g_fw_flag = 0;
        return;
    }
    ESP_LOGI(TAG, "HTTP Content Length: %d", content_length);

    const esp_partition_t *storage_partition  = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    if (storage_partition  == NULL) {
        ESP_LOGE(TAG, "Storage  partition not found");
        esp_http_client_cleanup(client);
        g_fw_flag = 0;
        return;
    }
    ESP_LOGI(TAG, "STORAGE PARTITION: %s", storage_partition->label);

    esp_partition_erase_range(storage_partition, 0, storage_partition->size);

    size_t write_offset = 0;
    char buffer[HTTPS_RESPONSE_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = esp_http_client_read(client, buffer, HTTPS_RESPONSE_BUFFER_SIZE)) > 0) {
        err = esp_partition_write(storage_partition, write_offset, (const void *)buffer, bytes_read);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_partition_write failed: %s", esp_err_to_name(err));
            esp_http_client_cleanup(client);
            g_fw_flag = 0;
            return;
        }
        write_offset += bytes_read;
        //ESP_LOGI(TAG, "DATA WRITE: %d", write_offset);
    }

    if (bytes_read < 0) {
        ESP_LOGE(TAG, "esp_http_client_read failed: %s", esp_err_to_name(bytes_read));
        esp_http_client_cleanup(client);
        g_fw_flag = 0;
        return;
    }

    ESP_LOGI(TAG, "FIRMWARE DOWNLOADED SUCCESSFULLY");
    esp_http_client_cleanup(client);
    g_fw_flag = 0;
    main_app_send_message(MAIN_APP_FW_DONWLOADED, 0, write_offset,NULL);
}

/** @} */
