extern char *node_config_txt;
						
esp_err_t spiffs_init();
esp_err_t spiffs_read_file(char *file_name, char *destination);
esp_err_t spiffs_write_file(char *file_name, char *source);
esp_err_t spiffs_json_get(char *json_str, char *key, char *value);