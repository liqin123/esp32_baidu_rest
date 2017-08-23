#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "baidu_rest.h"
#include "http.h"
#include "driver/gpio.h"

#define TAG "REST:"


#define GPIO_INPUT_IO_0     36
#define GPIO_INPUT_PIN_SEL  ((1<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
void baidu_rest_task(void *pvParameters)
{
	//PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[36], PIN_FUNC_GPIO); 
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO36_U, FUNC_GPIO36_GPIO36_0);
	//init gpio 
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	gpio_install_isr_service(0);
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_ANYEGDE;
    io_conf.pin_bit_mask = GPIO_SEL_36 ;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en=0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
	gpio_set_direction(36 , GPIO_MODE_INPUT);
    //change gpio intrrupt type for one pin
    //gpio_set_intr_type(36, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    

    //install gpio isr service
    
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(36, gpio_isr_handler,NULL);
 
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}