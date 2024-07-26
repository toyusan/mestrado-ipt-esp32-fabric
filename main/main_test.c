/*
*************************************************************************
* @file			main_test.c
* @brief		Source file for the main_test.c module.
* @details		This file contains the implementation of functions for 
*				the main_test.c module, including initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			20 de jul. de 2024
* @version		1.0.0
* @note			Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/
#include "sysconfig.h"
#include "main_test.h"
#include "main_app.h"
#include "esp_err.h"
#include "esp_log.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
/**
 * @brief Tag used for ESP serial console messages
 */
static const char TAG [] = "TEST"; 

/**
 * @brief Variable to count the test loop
 */
int8_t test_loop = TEST_LOOP;

/**
 * @brief Variable to control the test type
 */
test_type_e test_type = NO_TEST;

uint8_t test_state = 0;

/* Function prototypes ---------------------------------------------------*/

/* Public Functions ------------------------------------------------------*/

/**
 * @defgroup main_test.c Public Functions
 * @{
 */

/**
 * @brief Initialize all tests based on the defined configurations
 */
void main_test_init(void) {
	
	test_state = 0;
	
    if (CONFIDENTIALITY_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Confidentiality Test...");
        test_type = CONFIDENTIALITY_TEST;
    }
    if (AUTHENTICITY_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Authenticity Test...");
        test_type = AUTHENTICITY_TEST;
    }
    if (INTEGRITY_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Integrity Test...");
        test_type = INTEGRITY_TEST;
    }
    if (FAIL_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Fail Test...");
        test_type = FAIL_TEST;
    }
    if (POWER_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Power Test...");
        test_type = POWER_TEST;
    }
    if (UPDATE_TIME_TEST_ENABLED) {
        ESP_LOGI(TAG,"Initializing Update Time Test...");
        test_type = UPDATE_TIME_TEST;
    }
}

void main_test_update_log(char* msg_log){
	ESP_LOGI(TAG,"%s", msg_log);   
}

void main_test_update_loop(void){
	    ESP_LOGI(TAG,"TEST LOOP %d", test_loop);
		test_loop--;
		if(test_loop >= 0)
			main_app_send_message(MAIN_APP_RELOAD, 0 ,0, NULL);
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup main_test.c Private Functions
 * @{
 */

/*
 * @brief  
 * @param  
 * @return 
 */

/** @} */
