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
#include "version.h"

// APP Includes


/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/

// WiFi Configuration
#define PERSONAL_SSID "Apto 172"
#define PERSONAL_PASS "30082023"

// HTTPS Server definitions
#define URL_LEN 50
#define PAYLOAD_LEN 255

#define HTTPS_BLOCKCHAIN_SERVER_URL "https://18.230.239.105:3000"
//#define HTTPS_BLOCKCHAIN_SERVER_PORT "3000"

//http://<gateway-ec2-ip-address>:8080/ipfs/<cid-do-firmware>
#define HTTPS_IPFS_SERVER_URL "http://177.71.161.69:8080/ipfs/"

#define HTTPS_RESPONSE_BUFFER_SIZE 2048
#define HTTPS_RECEIVED_MSG_SUCCESS 200

// Defines for the Smart Contract interface
//#define ADDRESS_REGISTER_DEVICE ""HTTPS_BLOCKCHAIN_SERVER_URL":"HTTPS_BLOCKCHAIN_SERVER_PORT"/register-device"
#define ADDRESS_REGISTER_DEVICE ""HTTPS_BLOCKCHAIN_SERVER_URL"/register-device"
#define PAYLOAD_REGISTER_DEVICE "{\"hardwareVersion\": \"" HARDWARE_MODEL "\", \"softwareVersion\": \"" FIRMWARE_VERSION "\"}"

#define ERROR_HW_NOT_FOUND "ERROR: Hardware version not found"
#define VERSION_UPDATED "OK: No update needed"
#define VERSION_OUTDATED "Update available"

//Server certifies
extern const char ca_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const char ca_cert_pem_end[] asm("_binary_ca_cert_pem_end");

//Client certifies
extern const char client_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const char client_cert_pem_end[] asm("_binary_device_cert_pem_end");

// Client private key 
extern const char client_key_pem_start[] asm("_binary_device_key_pem_start");
extern const char client_key_pem_end[] asm("_binary_device_key_pem_end");

// Remote Firmware Meta Data
typedef struct {
	char status[50];
    char version[20];
    char author[50];
    char hardwareModel[50];
    char integrityHash[255];
    char timestamp[20];
    char description[255];
    char cid[255];
} firmware_metadata_info_t;

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
