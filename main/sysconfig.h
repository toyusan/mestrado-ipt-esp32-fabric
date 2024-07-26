/**
*************************************************************************
* @file       sysconfig.h
* @brief      Header file for the sysconfig.h module.
* @details    This file contains declarations and prototypes for the 
*             sysconfig.h module, which includes initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @date       15 de jun. de 2024
* @version    1.0.0
* @copyright  Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_SYSCONFIG_H_
#define MAIN_SYSCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "version.h"

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

/**
 * @brief Define if it has to use the real certificate or the tests ones
 */
#define AES_128 0

/**
 * @brief Prints the Decript process
 */
#define PRINT_INFO 0

/**
 * @brief WiFi Configuration SSID
 */
#define PERSONAL_SSID "Apto 172"
//#define PERSONAL_SSID "iPhone Airton"

/**
 * @brief WiFi Configuration Password
 */
#define PERSONAL_PASS "30082023"
//#define PERSONAL_PASS "2153818aa"

#if AES_128
#define KEY_SIZE 16
#else
#define KEY_SIZE 32
#endif

/**
 * @brief AES-128 key used for encryption
 */
 #if AES_128
#define AES_KEY {0x37, 0x2a, 0x0e, 0xd9, 0x4f, 0x6b, 0x42, 0xc5, 0xbc, 0x8d, 0x4e, 0x24, 0x25, 0x11, 0x04, 0x67}
//#define AES_KEY {0x47, 0x2a, 0x0e, 0xd9, 0x4f, 0x6b, 0x42, 0xc5, 0xbc, 0x8d, 0x4e, 0x24, 0x25, 0x11, 0x04, 0x67}
#else
#define AES_KEY {0x37, 0x2a, 0x0e, 0xd9, 0x4f, 0x6b, 0x42, 0xc5, 0xbc, 0x8d, 0x4e, 0x24, 0x25, 0x11, 0x04, 0x67, 0x37, 0x2a, 0x0e, 0xd9, 0x4f, 0x6b, 0x42, 0xc5, 0xbc, 0x8d, 0x4e, 0x24, 0x25, 0x11, 0x04, 0x67}
#endif
/**
 * @brief AES-128 IV used for encryption
 */
#define AES_IV {0x17, 0xfa, 0xfe, 0xb9, 0x31, 0x0a, 0x23, 0x16, 0x5d, 0x7f, 0x3d, 0x8f, 0xf5, 0x6c, 0x5f, 0x87}
//#define AES_IV {0x27, 0xfa, 0xfe, 0xb9, 0x31, 0x0a, 0x23, 0x16, 0x5d, 0x7f, 0x3d, 0x8f, 0xf5, 0x6c, 0x5f, 0x87}

/**
 * @brief Length of URL buffer
 */
#define URL_LEN 255

/**
 * @brief Length of payload buffer
 */
#define PAYLOAD_LEN 255

/**
 * @brief URL of the HTTPS Blockchain Server
 */
#define HTTPS_BLOCKCHAIN_SERVER_URL "https://18.230.239.105:3000"

/**
 * @brief URL of the HTTPS IPFS Server
 */
#define HTTPS_IPFS_SERVER_URL "http://177.71.161.69:8080/ipfs/"

/**
 * @brief Size of the HTTPS response buffer
 */
#define HTTPS_RESPONSE_BUFFER_SIZE 2048

/**
 * @brief HTTP status code for successful message receipt
 */
#define HTTPS_RECEIVED_MSG_SUCCESS 200

/**
 * @brief Address to register device on the Blockchain server
 */
#define ADDRESS_REGISTER_DEVICE ""HTTPS_BLOCKCHAIN_SERVER_URL"/register-device"

/**
 * @brief Payload format for registering a device
 */
#define PAYLOAD_REGISTER_DEVICE "{\"hardwareVersion\": \"" HARDWARE_MODEL "\", \"softwareVersion\": \"" FIRMWARE_VERSION "\"}"

/**
 * @brief Error message for hardware version not found
 */
#define ERROR_HW_NOT_FOUND "ERROR: Hardware version not found"

/**
 * @brief Message indicating firmware version is up-to-date
 */
#define VERSION_UPDATED "OK: No update needed"

/**
 * @brief Message indicating firmware version is outdated
 */
#define VERSION_OUTDATED "Update available"

/**
 * @brief Server certificate start marker
 */
extern const char ca_cert_pem_start[] asm("_binary_ca_cert_pem_start");

/**
 * @brief Server certificate end marker
 */
extern const char ca_cert_pem_end[] asm("_binary_ca_cert_pem_end");

/**
 * @brief Client certificate start marker
 */
extern const char client_cert_pem_start[] asm("_binary_device_cert_pem_start");

/**
 * @brief Client certificate end marker
 */
extern const char client_cert_pem_end[] asm("_binary_device_cert_pem_end");

/**
 * @brief Client private key start marker
 */
extern const char client_key_pem_start[] asm("_binary_device_key_pem_start");

/**
 * @brief Client private key end marker
 */
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
