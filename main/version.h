/**
*************************************************************************
* @file       version.h
* @brief      Header file for the version.h module.
* @details    This file contains declarations and prototypes for the 
*             version.h module, which includes initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @date       21 de jun. de 2024
* @version    1.0.0
* @copyright  Toyotech - All rights reserved
*************************************************************************
*/

#ifndef MAIN_VERSION_H_
#define MAIN_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/** 
 * @brief Hardware model identifier
 */
#define HARDWARE_MODEL    "ModelX"

/** 
 * @brief Firmware version identifier
 */
#define FIRMWARE_VERSION  "4.1"

/**
 * @brief Converts a macro value to a string
 * @param x Value to be converted to string
 */
#define STRINGIFY(x) #x

/**
 * @brief Helper macro to convert a macro value to a string
 * @param x Value to be converted
 */
#define TOSTRING(x)  STRINGIFY(x)

/**
 * @brief Header string used in application information
 */
#define APP_HEADER  "-----------------------------------------------------------\r\n"

/**
 * @brief Application version information string
 */
#define APP_VERSION "                      HARDWARE: " HARDWARE_MODEL "  \r\n \
                     FIRMWARE: " FIRMWARE_VERSION "\r\n \
                     Author:     Airton Toyofuku   \r\n \
                     TimeStamp:  2024-06-21-20:14  \r\n \
                     Description: Fabric Version   \r\n"

/* Public Function Prototypes -------------------------------------------------*/

/**
 * @defgroup version.h Public Functions
 * @{
 */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MAIN_VERSION_H_ */
