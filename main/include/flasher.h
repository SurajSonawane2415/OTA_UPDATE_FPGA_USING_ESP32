#ifndef _FLASHER_H
#define _FLASHER_H

#include <sys/param.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

esp_err_t flash(const char *file_name);

#endif