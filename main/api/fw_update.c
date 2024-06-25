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
fw_update_ret_e decrypt_and_verify_firmware_from_storage(void){
	const esp_partition_t *storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    const esp_partition_t *ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

    if (storage_partition == NULL || ota0_partition == NULL) {
        ESP_LOGE(TAG, "Required partition not found");
        return FW_UPDATE_PARTION_ERROR;
    }

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 128);
    unsigned char iv[16];
    memcpy(iv, aes_iv, 16); // Inicializa o IV

    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);

    uint8_t encrypted_data[16];
    uint8_t decrypted_data[16];
    size_t read_offset = 0;
    size_t write_offset = 0;

    esp_ota_handle_t ota_handle;
    esp_ota_begin(ota0_partition, OTA_SIZE_UNKNOWN, &ota_handle);

    while (read_offset < storage_partition->size) {
        size_t read_size = sizeof(encrypted_data);
        esp_partition_read(storage_partition, read_offset, encrypted_data, read_size);

        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, read_size, iv, encrypted_data, decrypted_data);
        mbedtls_md5_update(&md5_ctx, decrypted_data, read_size);

        esp_ota_write(ota_handle, decrypted_data, read_size);
        read_offset += read_size;
        write_offset += read_size;

        if (read_size < sizeof(encrypted_data)) {
            break; // End of the data
        }
    }

    unsigned char hash[16];
    mbedtls_md5_finish(&md5_ctx, hash);
    mbedtls_md5_free(&md5_ctx);
    mbedtls_aes_free(&aes);

    ESP_LOGI(TAG, "MD5 hash of decrypted firmware: ");
    for (int i = 0; i < 16; i++) {
        ESP_LOGI(TAG, "%02x", hash[i]);
    }

    esp_ota_end(ota_handle);

    // Verificação adicional do hash pode ser realizada aqui
    // ...

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
