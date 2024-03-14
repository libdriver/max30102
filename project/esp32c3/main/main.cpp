/**
 * @file main.cpp
 * @author Geng Yuchao (gengyuchao11@163.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#ifdef __cplusplus
}
#endif


#include "driver_max30102_fifo.h"
#include "driver_max30102_register_test.h"
#include "driver_max30102_fifo_test.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include <stdlib.h>


#define MAX30102_INTR_USE_GPIO_NUM GPIO_NUM_7
#define ESP_INTR_FLAG_DEFAULT 0

/**
 * @brief global var definition
 */
static uint8_t gs_flag;                    /**< flag */
static uint32_t gs_raw_red[32];            /**< raw red buffer */
static uint32_t gs_raw_ir[32];             /**< raw ir buffer */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< irq function address */


static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void max30102_intr_task(void* arg)
{
    gpio_num_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            g_gpio_irq();
            // printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

/**
 * @brief  gpio interrupt init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t gpio_interrupt_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1 << MAX30102_INTR_USE_GPIO_NUM;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(max30102_intr_task, "max30102_intr_task", 4096, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(MAX30102_INTR_USE_GPIO_NUM, gpio_isr_handler, (void*) MAX30102_INTR_USE_GPIO_NUM);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    return 0;

}

/**
 * @brief  gpio interrupt deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t gpio_interrupt_deinit(void)
{
    //remove isr handler for gpio number.
    gpio_isr_handler_remove(MAX30102_INTR_USE_GPIO_NUM);
    gpio_uninstall_isr_service();
    return 0;
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @note      none
 */
void max30102_receive_callback(uint8_t type)
{
    switch (type)
    {
        case MAX30102_INTERRUPT_STATUS_FIFO_FULL :
        {
            uint8_t res;
            uint8_t len;
            
            /* read data */
            len = 32;
            res = max30102_fifo_read((uint32_t *)gs_raw_red, (uint32_t *)gs_raw_ir, (uint8_t *)&len);
            if (res != 0)
            {
                max30102_interface_debug_print("max30102: read failed.\n");
            }
            max30102_interface_debug_print("max30102: irq fifo full with %d.\n", len);
            gs_flag = 1;
            
            break;
        }
        case MAX30102_INTERRUPT_STATUS_PPG_RDY :
        {
            max30102_interface_debug_print("max30102: irq ppg rdy.\n");
            
            break;
        }
        case MAX30102_INTERRUPT_STATUS_ALC_OVF :
        {
            max30102_interface_debug_print("max30102: irq alc ovf.\n");
            
            break;
        }
        case MAX30102_INTERRUPT_STATUS_PWR_RDY :
        {
            max30102_interface_debug_print("max30102: irq pwr rdy.\n");
            
            break;
        }
        case MAX30102_INTERRUPT_STATUS_DIE_TEMP_RDY :
        {
            max30102_interface_debug_print("max30102: irq die temp rdy.\n");
            
            break;
        }
        default :
        {
            max30102_interface_debug_print("max30102: unknow code.\n");
            
            break;
        }
    }
}

/**
 * @brief     max30102 full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 *             - 1 run failed
 *             - 5 param is invalid
 * @note      none
 */
uint8_t max30102(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        goto help;
    }
    else if (argc == 2)
    {
        if (strcmp("-i", argv[1]) == 0)
        {
            max30102_info_t info;
            
            /* print max30102 info */
            max30102_info(&info);
            max30102_interface_debug_print("max30102: chip is %s.\n", info.chip_name);
            max30102_interface_debug_print("max30102: manufacturer is %s.\n", info.manufacturer_name);
            max30102_interface_debug_print("max30102: interface is %s.\n", info.interface);
            max30102_interface_debug_print("max30102: driver version is %d.%d.\n", info.driver_version/1000, (info.driver_version%1000)/100);
            max30102_interface_debug_print("max30102: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
            max30102_interface_debug_print("max30102: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
            max30102_interface_debug_print("max30102: max current is %0.2fmA.\n", info.max_current_ma);
            max30102_interface_debug_print("max30102: max temperature is %0.1fC.\n", info.temperature_max);
            max30102_interface_debug_print("max30102: min temperature is %0.1fC.\n", info.temperature_min);
            
            return 0;
        }
        else if (strcmp("-p", argv[1]) == 0)
        {
            /* print pin connection */
            max30102_interface_debug_print("max30102: SCL connected to GPIO3(BCM).\n");
            max30102_interface_debug_print("max30102: SDA connected to GPIO2(BCM).\n");
            max30102_interface_debug_print("max30102: INT connected to GPIO17(BCM).\n");
            
            return 0;
        }
        else if (strcmp("-h", argv[1]) == 0)
        {
            /* show max30102 help */
            
            help:
            
            max30102_interface_debug_print("max30102 -i\n\tshow max30102 chip and driver information.\n");
            max30102_interface_debug_print("max30102 -h\n\tshow max30102 help.\n");
            max30102_interface_debug_print("max30102 -p\n\tshow max30102 pin connections of the current board.\n");
            max30102_interface_debug_print("max30102 -t reg\n\trun max30102 register test.\n");
            max30102_interface_debug_print("max30102 -t fifo <times>\n\trun max30102 fifo test.times means test times.\n");
            max30102_interface_debug_print("max30102 -c fifo <times>\n\trun max30102 fifo function.times means read times.\n");
            
            return 0;
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 3)
    {
        /* run test */
        if (strcmp("-t", argv[1]) == 0)
        {
            /* reg test */
            if (strcmp("reg", argv[2]) == 0)
            {
                uint8_t res;
                
                res = max30102_register_test();
                if (res != 0)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    else if (argc == 4)
    {
        /* run test */
        if (strcmp("-t", argv[1]) == 0)
        {
            /* reg test */
            if (strcmp("fifo", argv[2]) == 0)
            {
                uint8_t res;
                
                /* set gpio */
                g_gpio_irq = max30102_fifo_test_irq_handler;
                res = gpio_interrupt_init();
                if (res != 0)
                {
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                res = max30102_fifo_test(atoi(argv[3]));
                if (res != 0)
                {
                    (void)gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                else
                {
                    (void)gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 0;
                }
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        else if (strcmp("-c", argv[1]) == 0)
        {
            /* reg test */
            if (strcmp("fifo", argv[2]) == 0)
            {
                uint8_t res;
                uint32_t timeout;
                uint32_t cnt, times;
                
                /* get times */
                times = atoi(argv[3]);
                cnt = times;
                
                /* set gpio */
                g_gpio_irq = max30102_fifo_irq_handler;
                res = gpio_interrupt_init();
                if (res != 0)
                {
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                /* fifo init */
                res = max30102_fifo_init(max30102_receive_callback);
                if (res != 0)
                {
                    (void)gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                /* read data */
                gs_flag = 0;
                timeout = 5000;
                while (timeout != 0)
                {
                    if (gs_flag != 0)
                    {
                        max30102_interface_debug_print("max30102: %d/%d.\n", cnt - times + 1, cnt);
                        
                        /* clear config */
                        gs_flag = 0;
                        timeout = 5000;
                        times--;
                        if (times == 0)
                        {
                            break;
                        }
                    }
                    max30102_interface_delay_ms(1);
                    timeout--;
                }
                
                /* check timeout */
                if (timeout == 0)
                {
                    max30102_interface_debug_print("max30102: read timeout failed.\n");
                    (void)max30102_fifo_deinit();
                    (void)gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                   
                    return 1;
                }
                
                (void)max30102_fifo_deinit();
                (void)gpio_interrupt_deinit();
                g_gpio_irq = NULL;
                
                return 0;
            }
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    /* param is invalid */
    else
    {
        return 5;
    }
}



static struct {
    struct arg_str *info;
    struct arg_str *pin;
    struct arg_str *help;
    struct arg_str *test;
    struct arg_str *check;
    struct arg_end *end;
} cmd_max30102_args;

typedef uint8_t (*max30102_func_cb_t)(int argc, char **argv);

/**
 * @brief     cmd_max30102 function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 * @note      none
 */
static int cmd_max30102(int argc, char **argv)
{
    uint8_t res;

    res = max30102(argc, argv);
    if (res == 0)
    {
        /* run success */
    }
    else if (res == 1)
    {
        max30102_interface_debug_print("max30102: run failed.\n");
    }
    else if (res == 5)
    {
        max30102_interface_debug_print("max30102: param is invalid.\n");
    }
    else
    {
        max30102_interface_debug_print("max30102: unknow status code.\n");
    }

    return 0;
}

static void register_cmd_max30102(void)
{
    int num_args = 3;
    cmd_max30102_args.info =
        arg_str0("i", "info", "null", "info of max30102");
    cmd_max30102_args.pin =
        arg_str0("p", "pin", "null", "pin of max30102");
    cmd_max30102_args.help =
        arg_str0("h", "help", "null", "help of max30102");
    cmd_max30102_args.test =
        arg_strn("t", "test", "<reg,fifo reg>", 0 , 2, "test of max30102");
    cmd_max30102_args.check =
        arg_strn("c", "check", "<fifo reg>", 0 , 2, "check of max30102");

    cmd_max30102_args.end = arg_end(num_args);

    const esp_console_cmd_t cmd = {
        .command = "max30102",
        .help = "max30102 useful cmd.",
        .hint = NULL,
        .func = &cmd_max30102,
        .argtable = &cmd_max30102_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

extern "C" void register_max30102(void)
{
    register_cmd_max30102();
}

extern "C" void app_main(void)
{
    printf("Hello world!\n");

    void register_max30102(void);
    register_max30102();
    
    void console_main(void);
    console_main();
    
    while(1) {
        vTaskDelay(10);
    }
}

