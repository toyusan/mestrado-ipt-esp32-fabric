/*
*************************************************************************
* @file			sysconfig.h
* @brief		Header file for the sysconfig.h module.
* @details		This file contains declarations and prototypes for the 
*				sysconfig.h module,which includes initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			15 de jun. de 2024
* @version		1.0.0
* @copyright	Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_SYSCONFIG_H_
#define MAIN_SYSCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/
#define PERSONAL_SSID "Apto 172"
#define PERSONAL_PASS "30082023"

// HTTPS Server definitions
#define HTTPS_SERVER_URL " https://18.230.239.105"
#define HTTPS_SERVER_PORT 3000

//Server certifies
extern const char server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const char server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

//Client certifies
extern const char client_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const char client_cert_pem_end[] asm("_binary_device_cert_pem_end");

// Client private key 
extern const char client_key_pem_start[] asm("_binary_device_key_pem_start");
extern const char client_key_pem_end[] asm("_binary_device_key_pem_end");

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup sysconfig.h Public Functions
 * @{
 */

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_SYSCONFIG_H_ */
