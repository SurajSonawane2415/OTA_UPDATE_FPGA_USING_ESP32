#include "flasher.h"
#include <string.h>
#include <esp_ota_ops.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp32_port.h"
#include "esp_loader.h"
#include "example_common.h"

static const char *TAG = "serial_flasher";

#define HIGHER_BAUDRATE 230400
#define CHUNK_SIZE 1024
#define FILE_PATH_MAX 1024
#define BASE_PATH "/spiffs/"

// Function to get the size of a file
long get_file_size(FILE *file)
{
    // Save the current position
    long current_position = ftell(file);

    // Move to the end of the file
    fseek(file, 0, SEEK_END);

    // Get the current position, which is the size of the file
    long size = ftell(file);

    // Restore the original position
    fseek(file, current_position, SEEK_SET);

    return size;
}

esp_err_t flash(const char *file_name)
{
    ESP_LOGI(TAG, "Enter into flasher.c code");
    char file_path[FILE_PATH_MAX];
    sprintf(file_path, "%s%s", BASE_PATH, file_name);
    ESP_LOGI(TAG, "File name: %s", file_path);
    FILE *flash_file = fopen(file_path, "rb");

    if (flash_file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open firmware file");
        return ESP_FAIL;
    }

    example_binaries_t bin;

    const loader_esp32_config_t config = {
        .baud_rate = 115200,
        .uart_port = UART_NUM_1,
        .uart_rx_pin = GPIO_NUM_5,
        .uart_tx_pin = GPIO_NUM_4,
        .reset_trigger_pin = GPIO_NUM_25,
        .gpio0_trigger_pin = GPIO_NUM_26,
    };

    if (loader_port_esp32_init(&config) != ESP_LOADER_SUCCESS)
    {
        ESP_LOGE(TAG, "serial initialization failed.");
        fclose(flash_file);
        return ESP_FAIL;
    }

    uint8_t *buffer = (uint8_t *)malloc(CHUNK_SIZE);
    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for file buffer");
        fclose(flash_file);
        return ESP_FAIL;
    }

    size_t total_bytes_read = 0;
    size_t bytes_read;

    if (connect_to_target(HIGHER_BAUDRATE) == ESP_LOADER_SUCCESS)
    {
        get_example_binaries(esp_loader_get_target(), &bin);

        ESP_LOGI(TAG, "Loading bootloader...");
        flash_binary(bin.boot.data, bin.boot.size, bin.boot.addr);
        ESP_LOGI(TAG, "Loading partition table...");
        flash_binary(bin.part.data, bin.part.size, bin.part.addr);

        ESP_LOGI(TAG, "Loading app...");

        // Get the total size of the file
        size_t total_file_size = get_file_size(flash_file);
        size_t bytes_remaining = total_file_size;

        while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, flash_file)) > 0)
        {
            // Flash the chunk
            esp_loader_error_t err = flash_binary(buffer, bytes_read, bin.app.addr);
            if (err != ESP_LOADER_SUCCESS)
            {
                ESP_LOGE(TAG, "Error flashing binary file");
                free(buffer);
                fclose(flash_file);
                return ESP_FAIL;
            }
            total_bytes_read += bytes_read;
            bytes_remaining -= bytes_read;
            ESP_LOGI(TAG, "Remaining bytes: %d", bytes_remaining);
            if (bytes_remaining == 0) {
                // If all bytes are read from the file, exit the loop
                break;
            }
        }

        if (ferror(flash_file))
        {
            ESP_LOGE(TAG, "Error reading file");
            free(buffer);
            fclose(flash_file);
            return ESP_FAIL;
        }

        free(buffer);
        fclose(flash_file);

        ESP_LOGI(TAG, "File read and flashed successfully, total bytes read: %d", total_bytes_read);
        ESP_LOGI(TAG, "Done!");
    }

    return ESP_OK;
}