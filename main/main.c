/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "wm8978.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include <sys/socket.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "eth.h"
#include "event.h"
#include "wifi.h"
#include "hal_i2c.h"
#include "hal_i2s.h"
#include "wm8978.h"
#include "baidu_rest.h"
#include "http.h"
#include "cJSON.h"


#define GPIO_OUTPUT_IO_0    16
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_0))

#define TAG "main:"
// typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);
// typedef int (*http_cb) (http_parser*);


//char* http_body;
uint32_t http_body_length=0;
char* http_body=NULL;
static int body_callback(http_parser* a, const char *at, size_t length){
    http_body=realloc(http_body,http_body_length+length);
    memcpy(http_body+http_body_length,at,length);
    http_body_length+=length;
    // cJSON *root;
    // root= cJSON_Parse(http_body);
    // char* err_msg;
    // err_msg=cJSON_GetObjectItem(root,"err_msg")->valuestring;
    // ESP_LOGI(TAG,"err_msg:%s",err_msg);
    // char* sn;
    // err_msg=cJSON_GetObjectItem(root,"sn")->valuestring;
    // ESP_LOGI(TAG,"sn:%s",err_msg);
    // int err_no;
    // err_no=cJSON_GetObjectItem(root,"err_no")->valueint;
    // ESP_LOGI(TAG,"err_msg:%d",err_no);
    // cJSON_Delete(root);
    //printf("recv data:%s\n", http_body);
    //ESP_LOGI(TAG,"received body:%s",http_body);
    return 0;
}
int body_done_callback (http_parser* a){
    http_body=realloc(http_body,http_body_length+1);
    http_body[http_body_length]='\0';
    // ESP_LOGI(TAG,"return code:%d",a->status_code);
    // ESP_LOGI(TAG,"request method:%d",a->method);
    // ESP_LOGI(TAG,"data:%s",http_body);
    cJSON *root;
    root= cJSON_Parse(http_body);
    char* err_msg;
    err_msg=cJSON_GetObjectItem(root,"err_msg")->valuestring;
    ESP_LOGI(TAG,"err_msg:%s",err_msg);
    char* sn;
    err_msg=cJSON_GetObjectItem(root,"sn")->valuestring;
    ESP_LOGI(TAG,"sn:%s",err_msg);
    int err_no;
    err_no=cJSON_GetObjectItem(root,"err_no")->valueint;
    ESP_LOGI(TAG,"err_msg:%d",err_no);
    cJSON_Delete(root);
    ESP_LOGI(TAG,"received body:%s",http_body);
    free(http_body);
    return 0;
}

static http_parser_settings settings_null =
{   .on_message_begin = 0
    ,.on_header_field = 0
    ,.on_header_value = 0
    ,.on_url = 0
    ,.on_status = 0
    ,.on_body = body_callback
    ,.on_headers_complete = 0
    ,.on_message_complete = body_done_callback
    ,.on_chunk_header = 0
    ,.on_chunk_complete = 0
};

void app_main()
{
    esp_err_t err;
    event_engine_init();
    nvs_flash_init();
    // gpio_config_t io_conf;
    // io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // io_conf.pull_down_en = 0;
    // io_conf.pull_up_en = 0;
    // gpio_config(&io_conf);
    tcpip_adapter_init();
    wifi_init_sta();

    hal_i2c_init(0,5,17);
    hal_i2s_init(0,44100,16,2);
    WM8978_Init();
    WM8978_ADDA_Cfg(1,1); 
    WM8978_Input_Cfg(1,0,0);     
    WM8978_Output_Cfg(1,0); 
    WM8978_MIC_Gain(10);
    WM8978_AUX_Gain(0);
    WM8978_LINEIN_Gain(0);
    WM8978_SPKvol_Set(0);
    WM8978_HPvol_Set(20,20);
    WM8978_EQ_3D_Dir(1);
    WM8978_EQ1_Set(0,24);
    WM8978_EQ2_Set(0,24);
    WM8978_EQ3_Set(0,24);
    WM8978_EQ4_Set(0,24);
    WM8978_EQ5_Set(0,0);
    // sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    // sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    // esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    //     .format_if_mount_failed = true,
    //     .max_files = 5
    // };
    // sdmmc_card_t* card;
    // err = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    // if (err != ESP_OK) {
    //     if (err == ESP_FAIL) {
    //         printf("Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
    //     } else {
    //         printf("Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", err);
    //     }
    //     return;
    // }
    //sdmmc_card_print_printf(stdout, card);
    // /*eth_init();
    //do{
    //gpio_set_level(GPIO_OUTPUT_IO_0, 0);
    xEventGroupWaitBits(station_event_group,STA_GOTIP_BIT,pdTRUE,pdTRUE,portMAX_DELAY);
    //ESP_LOGI(TAG,"got ip address");
    //xEventGroupWaitBits(eth_event_group,ETH_GOTIP_BIT,pdTRUE,pdTRUE,portMAX_DELAY);
    //esp_err_t tcpip_adapter_get_ip_printf(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_printf_t *ip_printf);
    //gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    tcpip_adapter_ip_info_t ip;
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
    if (tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip) == 0) {
        ESP_LOGI(TAG, "~~~~~~~~~~~");
        ESP_LOGI(TAG, "ETHIP:"IPSTR, IP2STR(&ip.ip));
        ESP_LOGI(TAG, "ETHPMASK:"IPSTR, IP2STR(&ip.netmask));
        ESP_LOGI(TAG, "ETHPGW:"IPSTR, IP2STR(&ip.gw));
        ESP_LOGI(TAG, "~~~~~~~~~~~");
    }

    xTaskCreate(baidu_rest_task, "asr_task", 4096, NULL, 5, NULL);
        //xEventGroupWaitBits(eth_event_group,ETH_DISCONNECTED_BIT,pdTRUE,pdTRUE,portMAX_DELAY);
    //}while(1);
    //if(create_tcp_server(8080)!=ESP_OK){
      //  return;
    //}
    //mqtt_start(&settings);
    //xTaskCreate(vTelnetTask, "telnet_task", 2048, NULL, (tskIDLE_PRIORITY + 10), NULL);
    //char databuff[100]={0};
    //int len=0;
    //xTaskCreatePinnedToCore
    char samples_data[64];
    //memset(samples_data,0,1024);
    http_client_get("http://vop.baidu.com/server_api",&settings_null,NULL);
    uint8_t cnt=0;
    while(1){
        //gpio_set_level(GPIO_OUTPUT_IO_0, cnt%2);
        //memset(samples_data,0,1024);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "%s/n",samples_data);
        //aplay("/sdcard/test.wav");
        //hal_i2s_read(0,samples_data,256,portMAX_DELAY);
        //hal_i2s_write(0,samples_data,256,0);
        //vTaskDelay(5000 / portTICK_PERIOD_MS);
        cnt++;
    }
}

