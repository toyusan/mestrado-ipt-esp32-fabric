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
 * @brief Structure to hold firmware metadata information
 */
typedef struct {
    char status[50];         /**< Status of the firmware */
    char version[20];        /**< Firmware version */
    char author[50];         /**< Author of the firmware */
    char hardwareModel[50];  /**< Hardware model compatible with the firmware */
    char integrityHash[255]; /**< Integrity hash of the firmware */
    char timestamp[20];      /**< Timestamp of the firmware release */
    char description[255];   /**< Description of the firmware */
    char cid[255];           /**< CID of the firmware in IPFS */
} firmware_metadata_info_t;


/**
 * @brief Enumeration for firmware update return codes
 */
typedef enum fw_update_ret {
    FW_UPDATE_OK = 0,                /**< Firmware update successful */
    FW_UPDATE_PARTION_NOT_FOUND,
    FW_UPDATE_PARTION_NOT_INIT,
    FW_UPDATE_PARTION_READ_ERROR,
    FW_UPDATE_PARTION_WRITE_ERROR,
    FW_UPDATE_DECRYPT_ERROR,
    FW_UPDATE_PARTION_NOT_CLOSED,
    FW_UPDATE_HASH_ERROR,
    FW_UPDATE_SET_PARTION_BOOT_ERROR         /**< Error setting the boot partition */
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
fw_update_ret_e decrypt_firmware_from_storage(int len);

/**
 * @brief Calculates the SHA-256 hash of a firmware in the OTA partition.
 * @param len Length of the firmware.
 * @param hash_buffer Buffer where the calculated hash will be stored.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure.
 */
fw_update_ret_e calculate_sha256_hash_from_ota(const char *hash_buffer);

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
