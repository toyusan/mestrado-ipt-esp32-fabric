/*
*************************************************************************
* @file			main.c
* @brief		Source file for the main.c module.
* @details		This file contains the implementation of functions for 
*				the main.c module, including initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			9 de jun. de 2024
* @version		1.0.0
* @note			Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/

// Standard C Includes
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// ESP Includes
#include "nvs.h"
#include "nvs_flash.h"

// Application Includes
#include "wifi_app.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/

/* Function prototypes ---------------------------------------------------*/

/* Public Functions ------------------------------------------------------*/ 

/**
 * @defgroup main.c Public Functions
 * @{
 */
 
void app_main(void){
	
	// Initialize the NVS
	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	// Start WiFi
	wifi_app_start();
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup main.c Private Functions
 * @{
 */
 
 /** @} */