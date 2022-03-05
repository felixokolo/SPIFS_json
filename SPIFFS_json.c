#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_spiffs.h"
#include "SPIFFS_json.h"
#include "esp_err.h"
#include <string.h>

#define MAX_LINE_LENGTH 80

FILE* f;
static const char *TAG = "SPIFFS_json";
char *node_config_txt;
						

esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
	
	
esp_err_t spiffs_init(){
	
	node_config_txt = (char *) malloc(1024);
	sprintf(node_config_txt,	
	"{	\"Node ID\": 1,\n\
	\"AP SSID\": \"Pheelix\",\n\
	\"AP password\": \"felix2021\",\n\
	\"ST SSID\": \"My_SSID\",\n\
	\"ST password\": \"mypassword\",\n\
	\"Alias\": \"Kitchen Control\",\n\
	\"Channels\": 4,\n\
	\"Channels alias\": {\n\
		\"1\": \"Light 1\",\n\
		\"2\": \"Light 2\",\n\
		\"3\": \"Socket 1\",\n\
		\"4\": \"Socket 2\",\n\
		},\n\
	\"Scheduled ON\": {\n\
	},\n\
	\"Scheduled OFF\": {\n\
	}\n\
	}");
	
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
	
esp_err_t spiffs_read_file(char *file_name, char *destination){
	ESP_LOGI(TAG, "Reading file");
	char *dir_path;
	char line[MAX_LINE_LENGTH];
	dir_path = (char *) malloc(50);
    sprintf(dir_path, "/spiffs/%s", file_name);
	f = fopen((char*)dir_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
		fclose(f);
		return ESP_ERR_NOT_FOUND;
    }
	fgets(destination, MAX_LINE_LENGTH, f);
    while(fgets(line, MAX_LINE_LENGTH, f)){
		//strcat(destination, " \n");
		strcat(destination, line);
	}
    fclose(f);
	return ESP_OK;

}

esp_err_t spiffs_write_file(char *file_name, char *source){
	ESP_LOGI(TAG, "Opening file for writing");
    char *dir_path;
	dir_path = (char *) malloc(50);
	sprintf(dir_path, "/spiffs/%s", file_name);
	f = fopen((char*)dir_path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
		return ESP_FAIL;
    }
    fprintf(f, source);
    fclose(f);
	return ESP_OK;
}


esp_err_t spiffs_json_get(char *json_str, char *key, char *value){
	unsigned int c = 0, d =0;
	bool equal = false;
	char key_end = ' ';
	while(*(json_str+c) != '\0' && !equal){
		if(*(json_str+c) == '"'){
			//printf("found \" \n");
			d=0;
			c++;
			equal = true;
			while(*(key+d) != '\0' && equal){
				if(*(key+d) == *(json_str+c+d)){
					equal = true;
					//printf("found %c\n",*(json_str+c+d));
				}
				else
					equal = false;
				d++;
			}
			if(equal){
				if(*(json_str+c+d) != '"')
					equal = false;
				c++;
				while(*(json_str+c+d) == ' ' || *(json_str+c+d) == '\t')
					c++;
			}
		}
		else
			c++;
		
	}
	if(equal){
		c+=d+1;
		d=0;
		while(*(json_str+c) == ' ' || *(json_str+c) == '\t')
			c++;
		if(*(json_str+c) == '{')
			key_end = '}';
		else 
			if(*(json_str+c) == '"'){
				key_end = '"';
				c++;
			}
		else
			key_end = ',';
		while(*(json_str+c+d) != key_end){
			*(value+d) = *(json_str+c+d);
			d++;
		}
		if(key_end == '}'){
			*(value+d)= key_end;
			d++;
		}
			
		*(value+d)= '\0';
		return ESP_OK;
	}
	
	return ESP_FAIL;
}
	
// Read configuration from memory
// Modify certain configuration and save to memory
