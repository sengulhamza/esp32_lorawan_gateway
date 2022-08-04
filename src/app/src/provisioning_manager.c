#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"
#include "core/core_tasks.h"
#include "core/sx127x.h"
#include "app/lora_manager.h"
#include "app/provisioning_manager.h"

static const char *TAG = "provisioning_manager";

static esp_err_t provisioning_mngr_check_cmd(uint8_t packet_id)
{
    if (packet_id == LORA_PACKET_ID_PROVISING) {
        return ESP_OK;
    }
    ESP_LOGE(TAG, "Packet id is not provisioning packet id! Packet_id:0x%x", packet_id);
    return ESP_FAIL;
}

esp_err_t provisioning_mngr_approve_client(provisioning_t *provisioning_packet)
{
    // Save new client to approved clients list.
    // Use flash or nvs.
    ESP_LOG_BUFFER_HEX(TAG, provisioning_packet->global_dev_eui, sizeof(provisioning_packet->global_dev_eui));
    return ESP_OK;
}

esp_err_t provisioning_mngr_add_new_client(lora_tx_packet *lora_data, char *app_key)
{
    if (provisioning_mngr_check_cmd(lora_data->packet_id) != ESP_OK) {
        return ESP_FAIL;
    }
    provisioning_t *provisioning_packet = (provisioning_t *)lora_data->data;
    if (!strncmp((char *)&provisioning_packet->app_key, app_key, sizeof(provisioning_packet->app_key))) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, provisioning_packet->app_key, sizeof(provisioning_packet->app_key), ESP_LOG_WARN);
        provisioning_mngr_approve_client(provisioning_packet);
        return ESP_OK;
    }

    ESP_LOGE(TAG, "App key not matched! %s -- %s", (char *)&provisioning_packet->app_key, app_key);
    return ESP_FAIL;
}
