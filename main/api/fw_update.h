/**
*************************************************************************
* @file       fw_update.h
* @brief      Header file for the fw_update.h module.
* @details    This file contains declarations and prototypes for the 
*             fw_update.h module, which includes initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       24 de jun. de 2024
* @copyright  Toyotech - All rights reserved
*************************************************************************
*/
#ifndef MAIN_API_FW_UPDATE_H_
#define MAIN_API_FW_UPDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Public Macros -------------------------------------------------------------*/

/* Public Types --------------------------------------------------------------*/
/**
 * @brief Enumeration for firmware update return codes
 */
typedef enum fw_update_ret {
    FW_UPDATE_OK = 0,                /**< Firmware update successful */
    FW_UPDATE_PARTION_ERROR,         /**< Error with the firmware partition */
    FW_UPDATE_SET_BOOT_ERROR         /**< Error setting the boot partition */
} fw_update_ret_e;

/* Public Function Prototypes -------------------------------------------------*/
/**
 * @defgroup fw_update.h Public Functions
 * @{
 */

/**
 * @brief Decrypts and verifies the firmware from storage.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure
 */
fw_update_ret_e decrypt_and_verify_firmware_from_storage(void);

/**
 * @brief Applies the firmware update.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure
 */
fw_update_ret_e apply_firmware_update(void);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* MAIN_API_FW_UPDATE_H_ */
