/*
*************************************************************************
* @file			https_app.c
* @brief		Source file for the https_app.c module.
* @details		This file contains the implementation of functions for 
*				the https_app.c module, including initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			15 de jun. de 2024
* @version		1.0.0
* @note			Toyotech - All rights reserved
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

// APP Includes
#include "portmacro.h"
#include "tasks_common.h"
#include "https_app.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
static const char TAG [] = "https_app";

// Queue handle used to manipulate the main queue of events
static QueueHandle_t https_app_queue_handle;

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
BaseType_t https_app_send_message(https_app_message_e msgID, const char *url, const char *payload, int response_code, const char* response_message){
    https_app_queue_message_t msg;
    msg.msgID = msgID;
    msg.url = NULL;
    msg.payload = NULL;
    msg.response_message = NULL;
    
    if(url){
		 msg.url = strdup(url);
    }
    if(payload){
		 msg.payload = strdup(payload);
	}
    msg.response_code = response_code;
    if(response_message){
		msg.response_message = strdup(response_message);
	}
    BaseType_t result = xQueueSend(https_app_queue_handle, &msg, portMAX_DELAY);
    
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
                    
                case HTTPS_APP_MSG_RECEIVE_RESPONSE:
                	ESP_LOGI(TAG, "HTTPS_APP_MSG_RECEIVE_RESPONSE");
                	// Process the server response
                    ESP_LOGI(TAG, "Response Code: %d", msg.response_code);
                    ESP_LOGI(TAG, "Response Message: %s", msg.response_message);
                break;    
                
                 case HTTPS_APP_MSG_DOWNLOAD_IPFS:
                    ESP_LOGI(TAG, "HTTPS_APP_MSG_DOWNLOAD_IPFS");
                    // Implement IPFS download functionality
                    //https_app_download_ipfs(msg.url);
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

esp_err_t client_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (evt->data) {//if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
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
    // Configuração do cliente HTTP
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
        .use_global_ca_store = false, // Opcionalmente, desabilitar o uso da loja de CA global
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };

    // Inicialização do cliente HTTP
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Configuração do payload da requisição
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Envio da requisição
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        // Tratamento da resposta
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        char response_buffer[512];
        esp_http_client_read(client, response_buffer, sizeof(response_buffer));

        // Log da resposta
        ESP_LOGI(TAG, "HTTPS POST Status = %d, content_length = %d", status_code, content_length);
        if(content_length > 0)
        	ESP_LOGI(TAG, "Response: %s", response_buffer);
    } else {
        // Log do erro
        ESP_LOGE(TAG, "HTTPS POST request failed: %s", esp_err_to_name(err));
    }

    // Cleanup do cliente HTTP
    esp_http_client_cleanup(client);

    return err;
}



/** @} */
