/**
*******************************************************************************
* @file       wifi_app.h
* @brief      Header file for the wifi_app.h module.
* @details    This file contains declarations and prototypes for the 
*             wifi_app.h module, which includes initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       9 de jun. de 2024
* @copyright  Toyotech - All rights reserved
*******************************************************************************
*/
#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "portmacro.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "esp_netif.h"
#include "esp_wifi_types.h"

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

/**
 * @brief Callback typedef for WiFi connected event
 */
typedef void (*wifi_connected_event_callback_t)(void);

// WiFi application settings
#define MAX_SSID_LENGTH             32              /**< IEEE standard maximum SSID length */
#define MAX_PASSWORD_LENGTH         64              /**< IEEE standard maximum password length */
#define MAX_CONNECTION_RETRIES      5               /**< Maximum number of retries on disconnect */

/**
 * @brief Netif object for the station
 */
extern esp_netif_t* esp_netif_sta;

/**
 * @brief Netif object for the access point
 */
extern esp_netif_t* esp_netif_ap;

/**
 * @brief Message IDs for the WiFi application task
 * @note Expand this based on your application requirements
 */
typedef enum wifi_app_message {
    WIFI_APP_MSG_CONNECTING_STA = 0,            /**< Message ID for connecting to station */
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,          /**< Message ID for station connected and got IP */
    WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT, /**< Message ID for user requested station disconnect */
    WIFI_APP_MSG_LOAD_SAVED_CREDENTIALS,        /**< Message ID for loading saved credentials */
    WIFI_APP_MSG_STA_DISCONNECTED               /**< Message ID for station disconnected */
} wifi_app_message_e;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct wifi_app_queue_message {
    wifi_app_message_e msgID; /**< Message ID from the wifi_app_message_e enum */
} wifi_app_queue_message_t;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup wifi_app.h Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 * @note Expand the parameter list based on your requirements e.g. how you've expanded the wifi_app_queue_message_t.
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/**
 * @brief Starts the WiFi RTOS task
 */
void wifi_app_start(void);

/**
 * @brief Gets the WiFi configuration
 * @return Pointer to the WiFi configuration
 */
wifi_config_t* wifi_app_get_wifi_config(void);
 
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_WIFI_APP_H_ */
