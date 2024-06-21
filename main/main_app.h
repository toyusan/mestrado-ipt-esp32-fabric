/*
*************************************************************************
* @file			main_app.h
* @brief		Header file for the main_app.h module.
* @details		This file contains declarations and prototypes for the 
*				main_app.h module,which includes initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			20 de jun. de 2024
* @version		1.0.0
* @copyright	Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_MAIN_APP_H_
#define MAIN_MAIN_APP_H_

#include "portmacro.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/
/**
 * @brief Message IDs for the WiFi application task
 * @note Expand this based on your application requirements
 */
typedef enum main_app_message{
	 MAIN_APP_MSG_STA_CONNECTED,
	 MAIN_APP_MSG_STA_DISCONNECTED
} main_app_message_e;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct main_app_queue_message{
	main_app_message_e msgID;
} main_app_queue_message_t;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup main_app.h Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID messae ID from the wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFalse
 * @note Expand the parameter list based on your requirements e.g. how you've expanded the wifi_app_queue_message_t. 
 */
BaseType_t main_app_send_message(main_app_message_e msgID);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_MAIN_APP_H_ */
