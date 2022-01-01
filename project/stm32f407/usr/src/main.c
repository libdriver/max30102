/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      main.c
 * @brief     main source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-03-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/03/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_max30102_fifo.h"
#include "driver_max30102_register_test.h"
#include "driver_max30102_fifo_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
static uint8_t gs_flag;                    /**< flag */
static uint32_t gs_raw_red[32];            /**< raw read buffer */
static uint32_t gs_raw_ir[32];             /**< raw ir buffer */
uint8_t g_buf[256];                        /**< uart buffer */
uint16_t g_len;                            /**< uart buffer length */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< irq function address */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin is the gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        if (g_gpio_irq)
        {
            g_gpio_irq();
        }
    }
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 * @note      none
 */
uint8_t max30102_receive_callback(uint8_t type)
{
    switch (type)
    {
        case MAX30102_INTERRUPT_STATUS_FIFO_FULL :
        {
            volatile uint8_t res;
            volatile uint8_t len;
            
            /* read data */
            len = 32;
            res = max30102_fifo_read((uint32_t *)gs_raw_red, (uint32_t *)gs_raw_ir, (uint8_t *)&len);
            if (res)
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
            break;
        }
    }
    
    return 0;
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
            max30102_interface_debug_print("max30102: SCL connected to GPIOB PIN8.\n");
            max30102_interface_debug_print("max30102: SDA connected to GPIOB PIN9.\n");
            max30102_interface_debug_print("max30102: INT connected to GPIOB PIN0.\n");
            
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
                volatile uint8_t res;
                
                res = max30102_register_test();
                if (res)
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
                volatile uint8_t res;
                
                /* set gpio */
                g_gpio_irq = max30102_interface_fifo_test_irq_handler;
                res = gpio_interrupt_init();
                if (res)
                {
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                res = max30102_fifo_test(atoi(argv[3]));
                if (res)
                {
                    gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                else
                {
                    gpio_interrupt_deinit();
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
                volatile uint8_t res;
                volatile uint32_t timeout;
                volatile uint32_t cnt, times;
                
                /* get times */
                times = atoi(argv[3]);
                cnt = times;
                
                /* set gpio */
                g_gpio_irq = max30102_interface_fifo_irq_handler;
                res = gpio_interrupt_init();
                if (res)
                {
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                /* fifo init */
                res = max30102_fifo_init(max30102_receive_callback);
                if (res)
                {
                    gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                    
                    return 1;
                }
                
                /* read data */
                gs_flag = 0;
                timeout = 5000;
                while (timeout)
                {
                    if (gs_flag)
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
                    max30102_fifo_deinit();
                    gpio_interrupt_deinit();
                    g_gpio_irq = NULL;
                   
                    return 1;
                }
                
                max30102_fifo_deinit();
                gpio_interrupt_deinit();
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

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    volatile uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart1 init */
    uart1_init(115200);
    
    /* shell init && register max30102 fuction */
    shell_init();
    shell_register("max30102", max30102);
    uart1_print("max30102: welcome to libdriver max30102.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart1_read(g_buf, 256);
        if (g_len)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart1_print("max30102: run failed.\n");
            }
            else if (res == 2)
            {
                uart1_print("max30102: unknow command.\n");
            }
            else if (res == 3)
            {
                uart1_print("max30102: length is too long.\n");
            }
            else if (res == 4)
            {
                uart1_print("max30102: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart1_print("max30102: param is invalid.\n");
            }
            else
            {
                uart1_print("max30102: unknow status code.\n");
            }
            uart1_flush();
        }
        delay_ms(100);
    }
}
