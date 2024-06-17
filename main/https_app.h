/*
*************************************************************************
* @file			https_app.h
* @brief		Header file for the https_app.h module.
* @details		This file contains declarations and prototypes for the 
*				https_app.h module,which includes initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			15 de jun. de 2024
* @version		1.0.0
* @copyright	Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_HTTPS_APP_H_
#define MAIN_HTTPS_APP_H_

#include "portmacro.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

/**
 * @brief Message IDs for the HTTPS application task
 * @note Expand this based on your application requirements
 */
typedef enum https_app_message {
    HTTPS_APP_MSG_SEND_REQUEST = 0,
} https_app_message_e;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct https_app_queue_message {
    https_app_message_e msgID;
    const char *url;
    const char *payload;
} https_app_queue_message_t;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup https_app.h Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the https_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t https_app_send_message(https_app_message_e msgID, const char *url, const char *payload);

/**
 * @brief Starts the HTTPS RTOS task
 */
void https_app_start(void);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_HTTPS_APP_H_ */
