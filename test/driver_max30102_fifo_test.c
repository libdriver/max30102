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
 * @file      driver_max30102_fifo_test.c
 * @brief     driver max30102 fifo test source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-11-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/11/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_max30102_fifo_test.h"

static max30102_handle_t gs_handle;        /**< max30102 handle */
static volatile uint8_t gs_flag;           /**< flag */
static uint32_t gs_raw_red[32];            /**< raw red buffer */
static uint32_t gs_raw_ir[32];             /**< raw ir buffer */

/**
 * @brief  fifo test irq handler
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t max30102_fifo_test_irq_handler(void)
{
    /* run irq handler */
    if (max30102_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @note      none
 */
static void a_max30102_interface_test_receive_callback(uint8_t type)
{
    switch (type)
    {
        case MAX30102_INTERRUPT_STATUS_FIFO_FULL :
        {
            uint8_t res;
            uint8_t len;

            /* read data */
            len = 32;
            res = max30102_read(&gs_handle, (uint32_t *)gs_raw_red, (uint32_t *)gs_raw_ir, (uint8_t *)&len);
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
            max30102_interface_debug_print("max30102: unknown code.\n");

            break;
        }
    }
}

/**
 * @brief     fifo test
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t max30102_fifo_test(uint32_t times)
{
    uint8_t res;
    uint32_t timeout;
    uint16_t raw;
    float temp;
    max30102_bool_t enable;
    max30102_info_t info;

    /* link interface function */
    DRIVER_MAX30102_LINK_INIT(&gs_handle, max30102_handle_t);
    DRIVER_MAX30102_LINK_IIC_INIT(&gs_handle, max30102_interface_iic_init);
    DRIVER_MAX30102_LINK_IIC_DEINIT(&gs_handle, max30102_interface_iic_deinit);
    DRIVER_MAX30102_LINK_IIC_READ(&gs_handle, max30102_interface_iic_read);
    DRIVER_MAX30102_LINK_IIC_WRITE(&gs_handle, max30102_interface_iic_write);
    DRIVER_MAX30102_LINK_DELAY_MS(&gs_handle, max30102_interface_delay_ms);
    DRIVER_MAX30102_LINK_DEBUG_PRINT(&gs_handle, max30102_interface_debug_print);
    DRIVER_MAX30102_LINK_RECEIVE_CALLBACK(&gs_handle, a_max30102_interface_test_receive_callback);

    /* get information */
    res = max30102_info(&info);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: get info failed.\n");

        return 1;
    }
    else
    {
        /* print chip info */
        max30102_interface_debug_print("max30102: chip is %s.\n", info.chip_name);
        max30102_interface_debug_print("max30102: manufacturer is %s.\n", info.manufacturer_name);
        max30102_interface_debug_print("max30102: interface is %s.\n", info.interface);
        max30102_interface_debug_print("max30102: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        max30102_interface_debug_print("max30102: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        max30102_interface_debug_print("max30102: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        max30102_interface_debug_print("max30102: max current is %0.2fmA.\n", info.max_current_ma);
        max30102_interface_debug_print("max30102: max temperature is %0.1fC.\n", info.temperature_max);
        max30102_interface_debug_print("max30102: min temperature is %0.1fC.\n", info.temperature_min);
    }

    /* start read test */
    max30102_interface_debug_print("max30102: start fifo test.\n");

    /* init the max30102 */
    res = max30102_init(&gs_handle);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: init failed.\n");

        return 1;
    }

    /* enable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_TRUE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set fifo sample averaging */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_8);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set fifo roll */
    res = max30102_set_fifo_roll(&gs_handle, MAX30102_BOOL_TRUE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set fifo roll failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set fifo almost full */
    res = max30102_set_fifo_almost_full(&gs_handle, 0xF);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set fifo almost full failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set mode */
    res = max30102_set_mode(&gs_handle, MAX30102_MODE_SPO2);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set mode failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set spo2 adc range */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_SPO2_ADC_RANGE_4096);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set spo2 sample rate */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_100_HZ);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set adc resolution */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_ADC_RESOLUTION_18_BIT);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set led red pulse amplitude */
    res = max30102_set_led_red_pulse_amplitude(&gs_handle, 0x7F);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set led red pulse amplitude failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set led ir pulse amplitude */
    res = max30102_set_led_ir_pulse_amplitude(&gs_handle, 0x7F);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set led ir pulse amplitude failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_1, MAX30102_LED_RED);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_2, MAX30102_LED_IR);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_3, MAX30102_LED_NONE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_4, MAX30102_LED_NONE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* disable die temperature */
    res = max30102_set_die_temperature(&gs_handle, MAX30102_BOOL_FALSE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set die temperature failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, MAX30102_BOOL_TRUE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, MAX30102_BOOL_FALSE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, MAX30102_BOOL_TRUE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, MAX30102_BOOL_FALSE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* disable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_FALSE);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* get status */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_FIFO_FULL, &enable);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* get status */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_DIE_TEMP_RDY, &enable);
    if (res != 0)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        (void)max30102_deinit(&gs_handle);

        return 1;
    }
    
    /* read temperature */
    res = max30102_read_temperature(&gs_handle, (uint16_t *)&raw, (float *)&temp);
    if (res)
    {
        max30102_interface_debug_print("max30102: read temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: temperature is %0.4fC.\n", temp);
    
    /* read data */
    gs_flag = 0;
    timeout = 5000;
    while (timeout != 0)
    {
        if (gs_flag != 0)
        {
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
        (void)max30102_deinit(&gs_handle);

        return 1;
    }

    /* finish read test */
    max30102_interface_debug_print("max30102: finish fifo test.\n");
    (void)max30102_deinit(&gs_handle);

    return 0;
}
