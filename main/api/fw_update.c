/**
*************************************************************************
* @file       fw_update.c
* @brief      Source file for the fw_update.c module.
* @details    This file contains the implementation of functions for 
*             the fw_update.c module, including initialization, 
*             configuration, and control functions.
* @author     Airton Y. C. Toyofuku
* @version    1.0.0
* @date       24 de jun. de 2024
* @note       Toyotech - All rights reserved
*************************************************************************
*/

/* Includes -------------------------------------------------------------*/
// Include Systems configuration
#include "sysconfig.h"

// Standard C Includes
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

// FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

// ESP Includes
#include "esp_event.h"
#include "esp_interface.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include "mbedtls/aes.h"
#include "mbedtls/md5.h"

// Application Includes
#include "portmacro.h"
#include "main_app.h"
#include "fw_update.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
/**
 * @brief AES-128 key and IV used for encryption
 */
static const unsigned char aes_key[16] = AES_KEY;
static const unsigned char aes_iv[16] = AES_IV;

/**
 * @brief Tag used for ESP serial console messages
 */
static const char TAG [] = "fw_update"; 

/* Function prototypes ---------------------------------------------------*/

/* Public Functions ------------------------------------------------------*/

/**
 * @defgroup fw_update.c Public Functions
 * @{
 */

/**
 * @brief Decrypts and verifies the firmware from storage.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure
 */
fw_update_ret_e decrypt_and_verify_firmware_from_storage(int len){
	const esp_partition_t *storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    const esp_partition_t *ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

    if (storage_partition == NULL || ota0_partition == NULL) {
        ESP_LOGE(TAG, "Required partition not found");
        return FW_UPDATE_PARTION_ERROR;
    }
	ESP_LOGI(TAG, "Required partition found successfully");
	
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 128);
    unsigned char iv[16];
    memcpy(iv, aes_iv, 16); // Inicializa o IV


    uint8_t encrypted_data[16];
    uint8_t decrypted_data[16];
    size_t read_offset = 0;

    esp_ota_handle_t ota_handle;
    esp_err_t err = esp_ota_begin(ota0_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        return FW_UPDATE_PARTION_ERROR;
    }
	ESP_LOGI(TAG, "esp_ota_begin successfully");
	
    size_t read_size = sizeof(encrypted_data);
    while (read_offset <= len) {    
        err = esp_partition_read(storage_partition, read_offset, encrypted_data, read_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_partition_read failed: %s", esp_err_to_name(err));
            esp_ota_end(ota_handle);
            return FW_UPDATE_PARTION_READ_ERROR;
        }
        ESP_LOGI(TAG, "esp_partition_read successfully");
        
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, read_size, iv, encrypted_data, decrypted_data);
        err = esp_ota_write(ota_handle, decrypted_data, read_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            esp_ota_end(ota_handle);
            return FW_UPDATE_PARTION_WRITE_ERROR;
        }
        ESP_LOGI(TAG, "esp_ota_write successfully %d", read_offset);
        
        read_offset += read_size;
    }

    mbedtls_aes_free(&aes);
    ESP_LOGI(TAG, "mbedtls_aes_free");

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
        return FW_UPDATE_WRITE_ERROR;
    }
    
    ESP_LOGI(TAG, "Firmware decrypted and verified successfully");
    return FW_UPDATE_OK;
}

/**
 * @brief Applies the firmware update.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure
 */
fw_update_ret_e apply_firmware_update() {
    const esp_partition_t *ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    if (ota0_partition == NULL) {
        ESP_LOGE(TAG, "OTA0 partition not found");
        return FW_UPDATE_PARTION_ERROR;
    }

    esp_err_t err = esp_ota_set_boot_partition(ota0_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        return FW_UPDATE_SET_BOOT_ERROR;
    }

    ESP_LOGI(TAG, "Firmware update applied. Rebooting...");
    esp_restart();
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup fw_update.c Private Functions
 * @{
 */

/* @brief None */

/** @} */
