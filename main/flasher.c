#include "flasher.h"
#include <string.h>
#include <esp_ota_ops.h>
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "esp_flash";

#define FILE_PATH_MAX 128
#define BASE_PATH "/spiffs/"

esp_err_t err = ESP_FAIL;
esp_ota_handle_t ota_handle; //handle for an application OTA update
/*esp_ota_begin() returns a handle which is then used for subsequent calls to esp_ota_write() and esp_ota_end()*/
const esp_partition_t *next_empty_ota_partition = NULL;
char chunk[128];

void ota_begin(void);
void get_ota_app_partition(void);
void rollback(void);
void ota_write(size_t size);

esp_err_t flash(const char *file_name)
{

    ESP_LOGI(TAG, "Enter into flasher.c code");
    char file_path[FILE_PATH_MAX];
    sprintf(file_path, "%s%s", BASE_PATH, file_name);
    ESP_LOGI(TAG, "File name: %s", file_path);
    FILE *flash_file = fopen(file_path, "rb");

    if(flash_file == NULL){
        ESP_LOGE(TAG, "Failed to open firmware file");
        return err;
    }
    
    get_ota_app_partition();
    ota_begin();
    
    size_t read_bytes;
    while ((read_bytes = fread(chunk, 1, sizeof(chunk), flash_file)) > 0) // fread: to read binary files
    {
        ota_write(read_bytes);
    }

    fclose(flash_file);
    rollback();
    return ESP_OK;
}


void get_ota_app_partition()
{
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    printf("Currently running at which partition?: %s\n", running_partition->label);
    next_empty_ota_partition = esp_ota_get_next_update_partition(NULL); 
    printf("next empty partition is?: %s\n", next_empty_ota_partition->label);
    /*To get next OTA app partition which should be written with a new firmware.
    Passing null because starting from first.*/
}

void ota_begin()
{   
    ota_handle = 0; // Initialized with 0 
    esp_ota_begin(next_empty_ota_partition, OTA_SIZE_UNKNOWN, &ota_handle); // OTA start
    ESP_LOGI(TAG, "ESP OTA BEGIN SUCCESFULLY");
    /*If image size is not yet known, pass OTA_SIZE_UNKNOWN which will cause the entire partition to be erased.*/
} /*After ota begin allocates memory that remains in use until esp_ota_end() is called with the returned handle(ota_handle).*/

void ota_write(size_t size)
{
    
    if (ota_handle == NULL)
    {
        ESP_LOGE(TAG, "OTA handle is NULL. Abort OTA write.");
        return;
    }
    // Write OTA update data to partition.
    if (esp_ota_write(ota_handle, (const void *)chunk, size) != ESP_OK){
        ESP_LOGE(TAG, "ERROR AT FIRMWARE FLASHING");
    }
    else{
        ESP_LOGI(TAG, "Successful flashing firmware");
    }
}

void rollback()
{
    if (esp_ota_end(ota_handle) != ESP_OK){
        
        ESP_LOGE(TAG, "ERROR AT: Finish OTA update and validate newly written app image");
    }
    else{
        ESP_LOGI(TAG, "Finish OTA update and validate newly written app image");
    }

    if (esp_ota_set_boot_partition(next_empty_ota_partition) != ESP_OK){
        ESP_LOGI(TAG, "ERROR AT: Configure OTA data for a new boot partition");
    }
    else{
        ESP_LOGI(TAG, "Configure OTA data for a new boot partition &, restarting now!");
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_restart();
}
