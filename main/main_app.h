/**
*************************************************************************
* @file       main_app.h
* @brief      Header file for the main_app.h module.
* @details    This file contains declarations and prototypes for the 
*             main_app.h module, which includes initialization, 
*             configuration, and control functions.
* @date       20 de jun. de 2024
* @version    1.0.0
* @copyright  Toyotech - All rights reserved
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
typedef enum main_app_message {
    MAIN_APP_MSG_STA_CONNECTED,      /**< Station connected */
    MAIN_APP_MSG_STA_DISCONNECTED,   /**< Station disconnected */
    MAIN_APP_MSG_HTTPS_CONNECTED,    /**< HTTPS connected */
    MAIN_APP_MSG_HTTPS_RECEIVED,     /**< HTTPS message received */
    MAIN_APP_MSG_HTTPS_DISCONNECTED, /**< HTTPS disconnected */
    MAIN_APP_FW_DONWLOADED,           /**< Firmware downloaded */
    MAIN_APP_RELOAD
} main_app_message_e;

/**
 * @brief States for the main application
 */
typedef enum main_app_state {
    MAIN_APP_IDLE,           /**< Idle state */
    MAIN_APP_CHECK_FW,       /**< Checking firmware state */
    MAIN_APP_DOWNLOAD_FW,    /**< Downloading firmware state */
    MAIN_APP_DECRYPT_FW,
    MAIN_APP_UPDATE_STATUS,  /**< Updating status state */
} main_app_state_e;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.g. add another type and parameter as required
 */
typedef struct main_app_queue_message {
    main_app_message_e msgID; /**< Message ID from the main_app_message_e enum */
    int code;                 /**< Code associated with the message */
    int len;                  /**< Length of the data */
    const char* data;         /**< Pointer to the data */
} main_app_queue_message_t;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup main_app.h Public Functions
 * @{
 */

/**
 * @brief Sends a message to the queue
 * @param msgID Message ID from the main_app_message_e enum
 * @param code Code associated with the message
 * @param len Length of the data
 * @param data Pointer to the data
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 * @note Expand the parameter list based on your requirements e.g. how you've expanded the main_app_queue_message_t.
 */
BaseType_t main_app_send_message(main_app_message_e msgID, int code, int len, const char* data);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_MAIN_APP_H_ */
