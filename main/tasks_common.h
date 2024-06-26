/**
*************************************************************************
* @file       tasks_common.h
* @brief      Header file for the tasks_common.h module.
* @details    This file contains declarations and prototypes for the 
*             tasks_common.h module, which includes initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @date       9 de jun. de 2024
* @version    1.0.0
* @copyright  Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

/** 
 * @brief Stack size for the main application task
 */
#define MAIN_APP_TASK_STACK_SIZE        8192

/**
 * @brief Priority for the main application task
 */
#define MAIN_APP_TASK_PRIORITY          5

/** 
 * @brief Stack size for the WiFi application task
 */
#define WIFI_APP_TASK_STACK_SIZE        4096

/**
 * @brief Priority for the WiFi application task
 */
#define WIFI_APP_TASK_PRIORITY          5

/**
 * @brief Core ID for the WiFi application task
 */
#define WIFI_APP_TASK_CORE_ID           0

/** 
 * @brief Stack size for the HTTPS application task
 */
#define HTTPS_APP_TASK_STACK_SIZE       8192

/**
 * @brief Priority for the HTTPS application task
 */
#define HTTPS_APP_TASK_PRIORITY         5

/* Public Function Prototypes -------------------------------------------------*/

/**
 * @defgroup tasks_common.h Public Functions
 * @{
 */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MAIN_TASKS_COMMON_H_ */
