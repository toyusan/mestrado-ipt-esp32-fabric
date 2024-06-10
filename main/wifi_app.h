/*
*******************************************************************************
* @file			wifi_app.h
* @brief		Header file for the wifi_app.h module.
* @details		This file contains declarations and prototypes for the 
*				wifi_app.h module,which includes initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			9 de jun. de 2024
* @version		1.0.0
* @copyright	Toyotech - All rights reserved
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

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

// WiFi application settings
#define WIFI_AP_SSID				"ESP32_AP"		// AP Name
#define WIFI_AP_PASSWORD			"password"			// AP Password
#define WIFI_AP_CHANNEL				1				// AP Channel
#define WIFI_AP_SSID_HIDDEN			0				// AP Visibility
#define WIFI_AP_MAX_CONNECTIONS		5				// AP Max clients
#define WIFI_AP_BEACON_INTERVAL		100				// AP beacon: 100 miliseconds
#define WIFI_AP_IP					"192.168.0.1"	// AP default IP
#define WIFI_AP_GATEWAY				"192.168.0.1"	// AP default gatway. Same as IP
#define WIFI_AP_NETMASK				"255.255.255.0"	// AP netmask
#define WIFI_AP_BANDWIDTH			WIFI_BW_HT20	// AP Bandwidth 20MHz
#define WIFI_AP_STA_POWER_SAVE		WIFI_PS_NONE	// Power save not used
#define MAX_SSID_LENGTH				32				// IEEE standard maximum
#define MAX_PASSWORD_LENGTH			64				// IEEE standard maximum
#define MAX_CONNECTIONS_RETRIES		5				// Retry number on disconect

// netif object for the station and access point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/**
 * @brief Message IDs for the WiFi application task
 * @note Expand this based on your application requirements
 */
typedef enum wifi_app_message{
	 WIFI_APP_MSG_START_HTTP_SERVER = 0,
	 WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
	 WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
} wifi_app_message_e;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct wifi_app_queue_message{
	wifi_app_message_e msgID;
} wifi_app_queue_message_t;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup wifi_app.h Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID messae ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFalse
 * @note Expand the parameter list based on your requirements e.g. how you've expanded the wifi_app_queue_message_t. 
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/**
 * @brief Starts the WiFi RTOS task
 */
 void wifi_app_start (void);
 
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_WIFI_APP_H_ */
