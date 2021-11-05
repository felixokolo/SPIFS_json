#include <stdio.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_spiffs.h"
#include "SPIFFS_com.h"

FILE* f;

esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
	
	
esp_err_t spiffs_init(){
	
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	size_t total = 0, used = 0;
	if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
		return ret;
	}	
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
	
	return ret;
	
}
	
esp_err_t spiffs_read_file(char *file_name,, char *destination){
	ESP_LOGI(TAG, "Reading file");
    char *dir_path = sprintf(dir_path, "/spiffs/%s", &file_name);
	f = fopen(*dir_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
		f = fopen(*dir_path, "w");
		fclose(f);
		f = fopen(*dir_path, "w");
    }
    fgets(&destination, 1000, f);
    fclose(f);
	
}
	
// Read configuration from memory
// Modify certain configuration and save to memory
