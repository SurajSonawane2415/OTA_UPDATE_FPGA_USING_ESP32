#include "flasher.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "stm_flash";

#define FILE_PATH_MAX 128
#define BASE_PATH "/spiffs/"

esp_err_t flash(const char *file_name)
{
    esp_err_t err = ESP_FAIL;

    char file_path[FILE_PATH_MAX];
    sprintf(file_path, "%s%s", BASE_PATH, file_name);
    ESP_LOGI(TAG, "File name: %s", file_path);
    FILE *flash_file = fopen(file_path, "rb");

    if(flash_file == NULL){
        return err;
    }

    char chunk;
    //while(fgetc(chunk, sizeof(chunk), flash_file)) {
    for(int i = 0; i<9588; i++) {
        chunk = fgetc(flash_file);
        fputc(chunk, stdout);
    }

     fclose(flash_file);

    return ESP_OK;
}
//spi write to eprom using esp32?