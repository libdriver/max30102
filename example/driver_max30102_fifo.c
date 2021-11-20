/**
 * Copyright (C) LibDriver 2015-2021 All rights reserved
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
 * @file      driver_max30102_fifo.c
 * @brief     driver max30102 fifo source file
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

#include "driver_max30102_fifo.h"

static max30102_handle_t gs_handle;        /**< max30102 handle */

/**
 * @brief  fifo example irq handler
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t max30102_interface_fifo_irq_handler(void)
{
    /* run irq handler */
    if (max30102_irq_handler(&gs_handle))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     fifo example init
 * @param[in] *fifo_receive_callback points to a fifo receive callback
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t max30102_fifo_init(uint8_t (*fifo_receive_callback)(uint8_t type))
{
    volatile uint8_t res;
    max30102_bool_t enable;
    
    /* link interface function */
    DRIVER_MAX30102_LINK_INIT(&gs_handle, max30102_handle_t);
    DRIVER_MAX30102_LINK_IIC_INIT(&gs_handle, max30102_interface_iic_init);
    DRIVER_MAX30102_LINK_IIC_DEINIT(&gs_handle, max30102_interface_iic_deinit);
    DRIVER_MAX30102_LINK_IIC_READ(&gs_handle, max30102_interface_iic_read);
    DRIVER_MAX30102_LINK_IIC_WRITE(&gs_handle, max30102_interface_iic_write);
    DRIVER_MAX30102_LINK_DELAY_MS(&gs_handle, max30102_interface_delay_ms);
    DRIVER_MAX30102_LINK_DEBUG_PRINT(&gs_handle, max30102_interface_debug_print);
    DRIVER_MAX30102_LINK_RECEIVE_CALLBACK(&gs_handle, fifo_receive_callback);
    
    /* init the max30102 */
    res = max30102_init(&gs_handle);
    if (res)
    {
        max30102_interface_debug_print("max30102: init failed.\n");
       
        return 1;
    }
    
    /* enable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set fifo sample averaging */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_FIFO_DEFAULT_SAMPLE_AVERAGING);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set fifo roll */
    res = max30102_set_fifo_roll(&gs_handle, MAX30102_FIFO_DEFAULT_FIFO_ROLL);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo roll failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set fifo almost full */
    res = max30102_set_fifo_almost_full(&gs_handle, MAX30102_FIFO_DEFAULT_FIFO_ALMOST_FULL);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo almost full failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set mode */
    res = max30102_set_mode(&gs_handle, MAX30102_FIFO_DEFAULT_MODE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set spo2 adc range */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_FIFO_DEFAULT_SPO2_ADC_RANGE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set spo2 sample rate */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_FIFO_DEFAULT_SPO2_SAMPLE_RATE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set adc resolution */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_FIFO_DEFAULT_ADC_RESOLUTION);
    if (res)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set led red pulse amplitude */
    res = max30102_set_led_red_pulse_amplitude(&gs_handle, MAX30102_FIFO_DEFAULT_LED_RED_PULSE_AMPLITUDE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set led red pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set led ir pulse amplitude */
    res = max30102_set_led_ir_pulse_amplitude(&gs_handle, MAX30102_FIFO_DEFAULT_LED_IR_PULSE_AMPLITUDE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set led ir pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_1, MAX30102_FIFO_DEFAULT_SLOT1);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_2, MAX30102_FIFO_DEFAULT_SLOT2);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_3, MAX30102_FIFO_DEFAULT_SLOT3);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set slot */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_4, MAX30102_FIFO_DEFAULT_SLOT4);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set die temperature */
    res = max30102_set_die_temperature(&gs_handle, MAX30102_FIFO_DEFAULT_DIE_TEMPERATURE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set die temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, MAX30102_FIFO_DEFAULT_INTERRUPT_PPG_RDY_EN);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, MAX30102_FIFO_DEFAULT_INTERRUPT_ALC_OVF_EN);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* set interrupt */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, MAX30102_FIFO_DEFAULT_INTERRUPT_DIE_TEMP_RDY_EN);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* disable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* get status */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_FIFO_FULL, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    return 0;
}

/**
 * @brief  fifo example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t max30102_fifo_deinit(void)
{
    volatile uint8_t res;
    
    res = max30102_deinit(&gs_handle);
    if (res)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief         read the data
 * @param[out]    *raw_red points to a read raw data buffer
 * @param[out]    *raw_ir points to a ir raw data buffer
 * @param[in,out] *len points to a length buffer
 * @return        status code
 *                - 0 success
 *                - 1 read failed
 * @note          none
 */
uint8_t max30102_fifo_read(uint32_t *raw_red, uint32_t *raw_ir, uint8_t *len)
{
    volatile uint8_t res;
    
    res = max30102_read(&gs_handle, raw_red, raw_ir, len);
    if (res)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
