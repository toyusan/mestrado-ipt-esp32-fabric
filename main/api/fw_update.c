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
#include "api/fw_update.h"

/* Definitions ----------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------*/

/* Private variables -----------------------------------------------------*/
/**
 * @brief AES-128 key and IV used for encryption
 */
static const unsigned char aes_key[KEY_SIZE] = AES_KEY;
static const unsigned char aes_iv[16] = AES_IV;

static int g_read_offset = 0;

/**
 * @brief Tag used for ESP serial console messages
 */
static const char TAG [] = "fw_update"; 

/* Function prototypes ---------------------------------------------------*/
void hex_string_to_bytes(const char *hex_string, char *byte_array);

/* Public Functions ------------------------------------------------------*/

/**
 * @defgroup fw_update.c Public Functions
 * @{
 */

/**
 * @brief Decrypts and verifies the firmware from storage.
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure
 */
fw_update_ret_e decrypt_firmware_from_storage(int len){
	
	g_read_offset = 0;
	
	mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, KEY_SIZE * 8);
    unsigned char iv[16];
    memcpy(iv, aes_iv, 16); // Inicializa o IV

    uint8_t encrypted_data[16];
    uint8_t decrypted_data[16];
    size_t read_offset = 0;
    size_t read_size = sizeof(encrypted_data);
    size_t decrypted_size = read_size;
    
    esp_err_t err = ESP_OK;
	esp_ota_handle_t ota_handle;
	
	// Gets the pointers for the partitions
	const esp_partition_t *storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    const esp_partition_t *ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    if (storage_partition == NULL || ota0_partition == NULL) {
        ESP_LOGE(TAG, "Required partition not found");
        return FW_UPDATE_PARTION_NOT_FOUND;
    }
	ESP_LOGI(TAG, "Required partition found successfully");
	    
    // Initialize the OTA API
    err = esp_ota_begin(ota0_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        return FW_UPDATE_PARTION_NOT_INIT;
    }
	ESP_LOGI(TAG, "esp_ota_begin successfully");
	
	// Read data until the "len" size
    while (read_offset < len) {  
		
		// Getting the encrypted data from the storage partition  
        err = esp_partition_read(storage_partition, read_offset, encrypted_data, read_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_partition_read failed: %s", esp_err_to_name(err));
            esp_ota_end(ota_handle);
            return FW_UPDATE_PARTION_READ_ERROR;
        }
#if PRINT_INFO
        ESP_LOGI(TAG, "esp_partition_read: %d", read_offset);
#endif
        // Decrypt the data from the storage partition
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, read_size, iv, encrypted_data, decrypted_data);
		// Print the calculated hash for debugging

#if PRINT_INFO    	
    	ESP_LOGI(TAG, "encrypted_data:");
    	for (int i = 0; i < 16; i++) {
        	printf("%02x", encrypted_data[i]);
    	}
    	printf("\n");

    	// Print the expected hash for debugging
    	ESP_LOGI(TAG, "decrypted_data:");
    	for (int i = 0; i < 16; i++) {
        	printf("%02x", decrypted_data[i]);
    	}
    	printf("\n");
#endif // PRINT_INFO

       // If it is the last block, remove padding
		decrypted_size = read_size;
       if ((read_offset + read_size) >= len) {
            uint8_t padding_value = decrypted_data[decrypted_size - 1];
            if (padding_value > 0 && padding_value <= 16) {
				ESP_LOGI(TAG, "padding_value: %d", padding_value);
                decrypted_size -= padding_value;
            } else {
                ESP_LOGE(TAG, "Invalid padding value: %d", padding_value);
                esp_ota_end(ota_handle);
                return FW_UPDATE_DECRYPT_ERROR;
            }
        }
        
		// Wirte the data decrypted into the OTA partition        
        err = esp_ota_write(ota_handle, decrypted_data, decrypted_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            esp_ota_end(ota_handle);
            return FW_UPDATE_PARTION_WRITE_ERROR;
        }
#if PRINT_INFO
        ESP_LOGI(TAG, "esp_ota_write: %d", decrypted_size);
#endif
        // Update the offset to the next block
        read_offset += read_size;
    }

    g_read_offset = read_offset - read_size;
    
	// Release the aes api
    mbedtls_aes_free(&aes);
    ESP_LOGI(TAG, "mbedtls_aes_free");

	// End the ota process
    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
        return FW_UPDATE_PARTION_NOT_CLOSED;
    }
    
    ESP_LOGI(TAG, "Firmware decrypted successfully");
    return FW_UPDATE_OK;
}

/**
 * @brief Calculates the SHA-256 hash of a firmware in the OTA partition.
 * @param len Length of the firmware.
 * @param hash_buffer Buffer with the hash received from the server
 * @return fw_update_ret_e FW_UPDATE_OK on success, or an error code on failure.
 */
fw_update_ret_e calculate_sha256_hash_from_ota(const char *integrity_hash) {
	esp_err_t err = ESP_OK;
	char data[16];
    size_t read_offset = 0;
    size_t read_size = sizeof(data);
    
    char calculated_hash[32] = {0x00};
    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha256_init(&sha256_ctx);
    mbedtls_sha256_starts(&sha256_ctx, 0); // 0 para SHA-256
    
	// Convert the string into an array
	char expected_hash[32] = {0x00};
	hex_string_to_bytes(integrity_hash, expected_hash);
	
    // Obtém o ponteiro para a partição OTA
    const esp_partition_t *ota0_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    if (ota0_partition == NULL) {
        ESP_LOGE(TAG, "Required partition not found");
        return FW_UPDATE_PARTION_NOT_FOUND;
    }
    ESP_LOGI(TAG, "Required partition found successfully");

    // Read data from the partition
    while(read_offset < g_read_offset){
		// Lê os dados da partição OTA
        err = esp_partition_read(ota0_partition, read_offset, data, read_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_partition_read failed: %s", esp_err_to_name(err));
            return FW_UPDATE_PARTION_READ_ERROR;
        }
        // Atualiza o hash SHA-256 com os dados lidos
        mbedtls_sha256_update(&sha256_ctx, (const unsigned char*)data, read_size);
        
        // Atualiza o offset para o próximo bloco
        read_offset += read_size;
	}
    // Finaliza o cálculo do hash SHA-256
    mbedtls_sha256_finish(&sha256_ctx, (unsigned char*)calculated_hash);
    mbedtls_sha256_free(&sha256_ctx);

    // Print the calculated hash for debugging
    ESP_LOGI(TAG, "Calculated SHA-256 hash:");
    for (int i = 0; i < 32; i++) {
        printf("%02x", calculated_hash[i]);
    }
    printf("\n");

    // Print the expected hash for debugging
    ESP_LOGI(TAG, "Expected SHA-256 hash:");
    for (int i = 0; i < 32; i++) {
        printf("%02x", expected_hash[i]);
    }
    printf("\n");

    // Verify if the calculated hash matches the expected hash
    if (memcmp(calculated_hash, expected_hash, 32) != 0) {
        ESP_LOGE(TAG, "Hash mismatch");
        return FW_UPDATE_HASH_ERROR;
    }
        
    ESP_LOGI(TAG, "SHA-256 hash calculated successfully");
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
        return FW_UPDATE_PARTION_NOT_FOUND;
    }

    esp_err_t err = esp_ota_set_boot_partition(ota0_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        return FW_UPDATE_SET_PARTION_BOOT_ERROR;
    }

    ESP_LOGI(TAG, "Firmware update applied. Rebooting...");
    esp_restart();
    return FW_UPDATE_OK;
}

/** @} */

/* Private Functions -----------------------------------------------------*/

/**
 * @defgroup fw_update.c Private Functions
 * @{
 */

void hex_string_to_bytes(const char *hex_string, char *byte_array) {
    size_t len = strlen(hex_string);
    for (size_t i = 0; i < len; i += 2) {
        char byte_str[3] = {hex_string[i], hex_string[i+1], '\0'};
        byte_array[i / 2] = (uint8_t) strtol(byte_str, NULL, 16);
    }
}

/** @} */
