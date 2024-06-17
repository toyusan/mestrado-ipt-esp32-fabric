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
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t https_app_send_message(https_app_message_e msgID, const char *url, const char *payload) {
    https_app_queue_message_t msg;
    msg.msgID = msgID;
    msg.url = url;
    msg.payload = payload;
    return xQueueSend(https_app_queue_handle, &msg, portMAX_DELAY);
}

/**
 * @brief Starts the HTTPS RTOS task
 */
void https_app_start(void) {
    ESP_LOGI(TAG, "STARTING HTTPS APPLICATION");

    // Create message queue
    https_app_queue_handle = xQueueCreate(3, sizeof(https_app_queue_message_t));

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

                default:
                    ESP_LOGI(TAG, "Unknown message ID");
                    break;
            }
        }
    }
}

/**
 * @brief Internal function to perform the HTTPS request
 * @param url URL to send the request
 * @param payload Data to send
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
static esp_err_t https_app_perform_request(const char *url, const char *payload) {
    return ESP_OK;
}
/** @} */
