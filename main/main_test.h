/*
*************************************************************************
* @file			main_test.h
* @brief		Header file for the main_test.h module.
* @details		This file contains declarations and prototypes for the 
*				main_test.h module,which includes initialization, 
*				configuration, and control functions.
* @author   	Airton Y. C. Toyofuku
* @date			20 de jul. de 2024
* @version		1.0.0
* @copyright	Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_MAIN_TEST_H_
#define MAIN_MAIN_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/
/**
 * @brief Enable or disable each test
 */
#define CONFIDENTIALITY_TEST_ENABLED 0  /**< Enable Confidentiality Test */
#define AUTHENTICITY_TEST_ENABLED    0  /**< Enable Authenticity Test */
#define INTEGRITY_TEST_ENABLED       0  /**< Enable Integrity Test */
#define FAIL_TEST_ENABLED            0  /**< Enable Fail Test */
#define POWER_TEST_ENABLED           0  /**< Enable Power Test */
#define UPDATE_TIME_TEST_ENABLED     0  /**< Enable Update Time Test */


/**
 * @brief Parameter to control how many times each test will be performed
 */
#define TEST_LOOP 50  /**< Number of times each test will be performed */

/* Public Types --------------------------------------------------------------*/
/**
 * @brief Enumeration for each test process
 */
typedef enum test_type{
    NO_TEST = 0,            /**< No test selected */
    CONFIDENTIALITY_TEST,   /**< Confidentiality test */
    AUTHENTICITY_TEST,      /**< Authenticity test */
    INTEGRITY_TEST,         /**< Integrity test */
    FAIL_TEST,              /**< Fail test */
    POWER_TEST,             /**< Power consumption test */
    UPDATE_TIME_TEST        /**< Update time test */
} test_type_e;


/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup main_test.h Public Functions
 * @{
 */

/**
 * @brief Initialize all tests based on the defined configurations
 */
void main_test_init(void);

void main_test_update_log(char* msg_log);

void main_test_update_loop(void);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_MAIN_TEST_H_ */
