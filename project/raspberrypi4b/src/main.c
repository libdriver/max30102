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
#include "gpio.h"
#include <getopt.h>
#include <stdlib.h>

/**
 * @brief global var definition
 */
static volatile uint8_t gs_flag;           /**< flag */
static uint32_t gs_raw_red[32];            /**< raw red buffer */
static uint32_t gs_raw_ir[32];             /**< raw ir buffer */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< irq function address */

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
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t max30102(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"times", required_argument, NULL, 1},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknow";
    uint32_t times = 3;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* running times */
            case 1 :
            {
                /* set the times */
                times = atol(optarg);
                
                break;
            } 
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);
    
    /* run the function */
    if (strcmp("t_reg", type) == 0)
    {
        uint8_t res;
        
        /* run reg test */
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
    else if (strcmp("t_fifo", type) == 0)
    {
        uint8_t res;
        
        /* set gpio irq */
        g_gpio_irq = max30102_fifo_test_irq_handler;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* run fifo test */
        res = max30102_fifo_test(times);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
            
            return 1;
        }
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_fifo", type) == 0)
    {
        uint8_t res;
        uint32_t timeout;
        uint32_t cnt;
        
        /* get times */
        cnt = times;
        
        /* set gpio irq */
        g_gpio_irq = max30102_fifo_irq_handler;
        
        /* gpio init */
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
        
        /* param init  */
        gs_flag = 0;
        timeout = 5000;
        
        /* loop */
        while (timeout != 0)
        {
            /* check the flag */
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
            
            /* delay 1ms */
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
        
        /* deinit */
        (void)max30102_fifo_deinit();
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0; 
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        max30102_interface_debug_print("Usage:\n");
        max30102_interface_debug_print("  max30102 (-i | --information)\n");
        max30102_interface_debug_print("  max30102 (-h | --help)\n");
        max30102_interface_debug_print("  max30102 (-p | --port)\n");
        max30102_interface_debug_print("  max30102 (-t reg | --test=reg)\n");
        max30102_interface_debug_print("  max30102 (-t fifo | --test=fifo) [--times=<num>]\n");
        max30102_interface_debug_print("  max30102 (-e fifo | --example=fifo) [--times=<num>]\n");
        max30102_interface_debug_print("\n");
        max30102_interface_debug_print("Options:\n");
        max30102_interface_debug_print("  -e <fifo>, --example=<fifo>    Run the driver example.\n");
        max30102_interface_debug_print("  -h, --help                     Show the help.\n");
        max30102_interface_debug_print("  -i, --information              Show the chip information.\n");
        max30102_interface_debug_print("  -p, --port                     Display the pin connections of the current board.\n");
        max30102_interface_debug_print("  -t <reg | fifo>, --test=<reg | fifo>\n");
        max30102_interface_debug_print("                                 Run the driver test.\n");
        max30102_interface_debug_print("      --times=<num>              Set the running times.([default: 3])\n");
        
        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        max30102_info_t info;
        
        /* print max30102 info */
        max30102_info(&info);
        max30102_interface_debug_print("max30102: chip is %s.\n", info.chip_name);
        max30102_interface_debug_print("max30102: manufacturer is %s.\n", info.manufacturer_name);
        max30102_interface_debug_print("max30102: interface is %s.\n", info.interface);
        max30102_interface_debug_print("max30102: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        max30102_interface_debug_print("max30102: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        max30102_interface_debug_print("max30102: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        max30102_interface_debug_print("max30102: max current is %0.2fmA.\n", info.max_current_ma);
        max30102_interface_debug_print("max30102: max temperature is %0.1fC.\n", info.temperature_max);
        max30102_interface_debug_print("max30102: min temperature is %0.1fC.\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        max30102_interface_debug_print("max30102: SCL connected to GPIO3(BCM).\n");
        max30102_interface_debug_print("max30102: SDA connected to GPIO2(BCM).\n");
        max30102_interface_debug_print("max30102: INT connected to GPIO17(BCM).\n");
        
        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief     main function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 * @note      none
 */
int main(uint8_t argc, char **argv)
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
