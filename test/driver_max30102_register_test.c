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
 * @file      driver_max30102_register_test.c
 * @brief     driver max30102 register test source file
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

#include "driver_max30102_register_test.h"
#include <stdlib.h>

static max30102_handle_t gs_handle;        /**< max30102 handle */

/**
 * @brief  register test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t max30102_register_test(void)
{
    volatile uint8_t res;
    volatile uint8_t pointer;
    volatile uint8_t pointer_check;
    volatile uint8_t value;
    volatile uint8_t value_check;
    volatile uint8_t revision_id;
    volatile uint8_t part_id;
    max30102_info_t info;
    max30102_bool_t enable;
    max30102_adc_resolution_t resolution;
    max30102_sample_averaging_t sample;
    max30102_mode_t mode;
    max30102_spo2_adc_range_t range;
    max30102_spo2_sample_rate_t rate;
    max30102_led_t led;
    
    /* link interface function */
    DRIVER_MAX30102_LINK_INIT(&gs_handle, max30102_handle_t);
    DRIVER_MAX30102_LINK_IIC_INIT(&gs_handle, max30102_interface_iic_init);
    DRIVER_MAX30102_LINK_IIC_DEINIT(&gs_handle, max30102_interface_iic_deinit);
    DRIVER_MAX30102_LINK_IIC_READ(&gs_handle, max30102_interface_iic_read);
    DRIVER_MAX30102_LINK_IIC_WRITE(&gs_handle, max30102_interface_iic_write);
    DRIVER_MAX30102_LINK_DELAY_MS(&gs_handle, max30102_interface_delay_ms);
    DRIVER_MAX30102_LINK_DEBUG_PRINT(&gs_handle, max30102_interface_debug_print);
    DRIVER_MAX30102_LINK_RECEIVE_CALLBACK(&gs_handle, max30102_interface_receive_callback);
    
    /* get information */
    res = max30102_info(&info);
    if (res)
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
    
    /* start register test */
    max30102_interface_debug_print("max30102: start register test.\n");
    
    /* init the max30102 */
    res = max30102_init(&gs_handle);
    if (res)
    {
        max30102_interface_debug_print("max30102: init failed.\n");
       
        return 1;
    }
    
    /* max30102_set_interrupt/max30102_get_interrupt test */
    max30102_interface_debug_print("max30102: max30102_set_interrupt/max30102_get_interrupt test.\n");
    
    /* enable fifo full */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable fifo full.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable fifo full */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable fifo full.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_FIFO_FULL_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* enable ppg ready */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable ppg ready.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable ppg ready */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable ppg ready.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_PPG_RDY_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* enable alc ovf */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable alc ovf.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable alc ovf */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable alc ovf.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_ALC_OVF_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* enable die temp ready */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable die temp ready.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable die temp ready */
    res = max30102_set_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable die temp ready.\n");
    res = max30102_get_interrupt(&gs_handle, MAX30102_INTERRUPT_DIE_TEMP_RDY_EN, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check interrupt %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* max30102_set_fifo_write_pointer/max30102_get_fifo_write_pointer test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_write_pointer/max30102_get_fifo_write_pointer test.\n");
    
    pointer = rand() % 0x1F;
    res = max30102_set_fifo_write_pointer(&gs_handle, pointer);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo write pointer failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set fifo write pointer %d.\n", pointer);
    res = max30102_get_fifo_write_pointer(&gs_handle, (uint8_t *)&pointer_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo write pointer failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* max30102_set_fifo_overflow_counter/max30102_get_fifo_overflow_counter test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_overflow_counter/max30102_get_fifo_overflow_counter test.\n");
    
    pointer = rand() % 0x1F;
    res = max30102_set_fifo_overflow_counter(&gs_handle, pointer);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo overflow counter failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set fifo overflow counter %d.\n", pointer);
    res = max30102_get_fifo_overflow_counter(&gs_handle, (uint8_t *)&pointer_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo overflow counter failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* max30102_set_fifo_read_pointer/max30102_get_fifo_read_pointer test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_read_pointer/max30102_get_fifo_read_pointer test.\n");
    
    pointer = rand() % 0x1F;
    res = max30102_set_fifo_read_pointer(&gs_handle, pointer);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo read pointer failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set fifo read pointer %d.\n", pointer);
    res = max30102_get_fifo_read_pointer(&gs_handle, (uint8_t *)&pointer_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo read pointer failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* max30102_set_fifo_data/max30102_get_fifo_data test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_data/max30102_get_fifo_data test.\n");
    
    pointer = rand() % 0x1F;
    res = max30102_set_fifo_data(&gs_handle, pointer);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo data failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set fifo data %d.\n", pointer);
    res = max30102_get_fifo_data(&gs_handle, (uint8_t *)&pointer_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo data failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    
    /* max30102_set_fifo_sample_averaging/max30102_get_fifo_sample_averaging test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_sample_averaging/max30102_get_fifo_sample_averaging test.\n");
    
    /* set sample averaging 1 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_1);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 1.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_1 ? "ok" : "error");
    
    /* set sample averaging 2 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_2);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 2.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_2 ? "ok" : "error");
    
    /* set sample averaging 4 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_4);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 4.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_4 ? "ok" : "error");
    
    /* set sample averaging 8 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_8);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 8.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_8 ? "ok" : "error");
    
    /* set sample averaging 16 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_16);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 16.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_16 ? "ok" : "error");
    
    /* set sample averaging 32 */
    res = max30102_set_fifo_sample_averaging(&gs_handle, MAX30102_SAMPLE_AVERAGING_32);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set sample averaging 32.\n");
    res = max30102_get_fifo_sample_averaging(&gs_handle, &sample);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo sample averaging failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check sample %s.\n", sample == MAX30102_SAMPLE_AVERAGING_32 ? "ok" : "error");
    
    /* max30102_set_fifo_roll/max30102_get_fifo_roll test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_roll/max30102_get_fifo_roll test.\n");
    
    /* enable fifo roll */
    res = max30102_set_fifo_roll(&gs_handle, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo roll failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable fifo roll.\n");
    res = max30102_get_fifo_roll(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo roll failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check roll %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable fifo roll */
    res = max30102_set_fifo_roll(&gs_handle, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo roll failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable fifo roll.\n");
    res = max30102_get_fifo_roll(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo roll failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check roll %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* max30102_set_fifo_almost_full/max30102_get_fifo_almost_full test */
    max30102_interface_debug_print("max30102: max30102_set_fifo_almost_full/max30102_get_fifo_almost_full test.\n");
    
    value = rand() % 0xF;
    res = max30102_set_fifo_almost_full(&gs_handle, value);
    if (res)
    {
        max30102_interface_debug_print("max30102: set fifo almost full failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set fifo almost full %d.\n", value);
    res = max30102_get_fifo_almost_full(&gs_handle, (uint8_t *)&value_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get fifo almost full failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check fifo almost full %s.\n", value_check == value ? "ok" : "error");
    
    /* max30102_set_shutdown/max30102_get_shutdown test */
    max30102_interface_debug_print("max30102: max30102_set_shutdown/max30102_get_shutdown test.\n");
    
    /* enable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable shutdown.\n");
    res = max30102_get_shutdown(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check shutdown %s.\n", enable == MAX30102_BOOL_TRUE ? "ok" : "error");
    
    /* disable shutdown */
    res = max30102_set_shutdown(&gs_handle, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable shutdown.\n");
    res = max30102_get_shutdown(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get shutdown failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check shutdown %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* max30102_set_mode/max30102_get_mode test */
    max30102_interface_debug_print("max30102: max30102_set_mode/max30102_get_mode test.\n");
    
    /* heart rate */
    res = max30102_set_mode(&gs_handle, MAX30102_MODE_HEART_RATE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set heart rate mode.\n");
    res = max30102_get_mode(&gs_handle, &mode);
    if (res)
    {
        max30102_interface_debug_print("max30102: get mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check mode %s.\n", mode == MAX30102_MODE_HEART_RATE ? "ok" : "error");
    
    /* spo2 */
    res = max30102_set_mode(&gs_handle, MAX30102_MODE_SPO2);
    if (res)
    {
        max30102_interface_debug_print("max30102: set mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 mode.\n");
    res = max30102_get_mode(&gs_handle, &mode);
    if (res)
    {
        max30102_interface_debug_print("max30102: get mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check mode %s.\n", mode == MAX30102_MODE_SPO2 ? "ok" : "error");
    
    /* multi led */
    res = max30102_set_mode(&gs_handle, MAX30102_MODE_MULTI_LED);
    if (res)
    {
        max30102_interface_debug_print("max30102: set mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set multi led mode.\n");
    res = max30102_get_mode(&gs_handle, &mode);
    if (res)
    {
        max30102_interface_debug_print("max30102: get mode failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check mode %s.\n", mode == MAX30102_MODE_MULTI_LED ? "ok" : "error");
    
    /* max30102_set_spo2_adc_range/max30102_get_spo2_adc_range test */
    max30102_interface_debug_print("max30102: max30102_set_spo2_adc_range/max30102_get_spo2_adc_range test.\n");
    
    /* 2048 */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_SPO2_ADC_RANGE_2048);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 adc range 2048.\n");
    res = max30102_get_spo2_adc_range(&gs_handle, &range);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 adc range %s.\n", range == MAX30102_SPO2_ADC_RANGE_2048 ? "ok" : "error");
    
    /* 4096 */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_SPO2_ADC_RANGE_4096);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 adc range 4096.\n");
    res = max30102_get_spo2_adc_range(&gs_handle, &range);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 adc range %s.\n", range == MAX30102_SPO2_ADC_RANGE_4096 ? "ok" : "error");
    
    /* 8192 */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_SPO2_ADC_RANGE_8192);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 adc range 8192.\n");
    res = max30102_get_spo2_adc_range(&gs_handle, &range);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 adc range %s.\n", range == MAX30102_SPO2_ADC_RANGE_8192 ? "ok" : "error");
    
    /* 16384 */
    res = max30102_set_spo2_adc_range(&gs_handle, MAX30102_SPO2_ADC_RANGE_16384);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 adc range 16384.\n");
    res = max30102_get_spo2_adc_range(&gs_handle, &range);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 adc range failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 adc range %s.\n", range == MAX30102_SPO2_ADC_RANGE_16384 ? "ok" : "error");
    
    /* max30102_set_spo2_sample_rate/max30102_get_spo2_sample_rate test */
    max30102_interface_debug_print("max30102: max30102_set_spo2_sample_rate/max30102_get_spo2_sample_rate test.\n");
    
    /* 50Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_50_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 50Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_50_HZ ? "ok" : "error");
    
    /* 100Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_100_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 100Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_100_HZ ? "ok" : "error");
    
    /* 200Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_200_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 200Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_200_HZ ? "ok" : "error");
    
    /* 400Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_400_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 400Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_400_HZ ? "ok" : "error");
    
    /* 800Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_800_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 800Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_800_HZ ? "ok" : "error");
    
    /* 1000Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_1000_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 1000Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_1000_HZ ? "ok" : "error");
    
    /* 1600Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_1600_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 1600Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_1600_HZ ? "ok" : "error");
    
    /* 3200Hz */
    res = max30102_set_spo2_sample_rate(&gs_handle, MAX30102_SPO2_SAMPLE_RATE_3200_HZ);
    if (res)
    {
        max30102_interface_debug_print("max30102: set spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set spo2 sample rate 3200Hz.\n");
    res = max30102_get_spo2_sample_rate(&gs_handle, &rate);
    if (res)
    {
        max30102_interface_debug_print("max30102: get spo2 sample rate failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check spo2 sample rate %s.\n", rate == MAX30102_SPO2_SAMPLE_RATE_3200_HZ ? "ok" : "error");
    
    /* max30102_set_adc_resolution/max30102_get_adc_resolution test */
    max30102_interface_debug_print("max30102: max30102_set_adc_resolution/max30102_get_adc_resolution test.\n");
    
    /* 15 bits */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_ADC_RESOLUTION_15_BIT);
    if (res)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set adc resolution 15 bits.\n");
    res = max30102_get_adc_resolution(&gs_handle, &resolution);
    if (res)
    {
        max30102_interface_debug_print("max30102: get adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check adc resolution %s.\n", resolution == MAX30102_ADC_RESOLUTION_15_BIT ? "ok" : "error");
    
    /* 16 bits */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_ADC_RESOLUTION_16_BIT);
    if (res)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set adc resolution 16 bits.\n");
    res = max30102_get_adc_resolution(&gs_handle, &resolution);
    if (res)
    {
        max30102_interface_debug_print("max30102: get adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check adc resolution %s.\n", resolution == MAX30102_ADC_RESOLUTION_16_BIT ? "ok" : "error");
    
    /* 17 bits */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_ADC_RESOLUTION_17_BIT);
    if (res)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set adc resolution 17 bits.\n");
    res = max30102_get_adc_resolution(&gs_handle, &resolution);
    if (res)
    {
        max30102_interface_debug_print("max30102: get adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check adc resolution %s.\n", resolution == MAX30102_ADC_RESOLUTION_17_BIT ? "ok" : "error");
    
    /* 18 bits */
    res = max30102_set_adc_resolution(&gs_handle, MAX30102_ADC_RESOLUTION_18_BIT);
    if (res)
    {
        max30102_interface_debug_print("max30102: set adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set adc resolution 18 bits.\n");
    res = max30102_get_adc_resolution(&gs_handle, &resolution);
    if (res)
    {
        max30102_interface_debug_print("max30102: get adc resolution failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check adc resolution %s.\n", resolution == MAX30102_ADC_RESOLUTION_18_BIT ? "ok" : "error");
    
    /* max30102_set_led_red_pulse_amplitude/max30102_get_led_red_pulse_amplitude test */
    max30102_interface_debug_print("max30102: max30102_set_led_red_pulse_amplitude/max30102_get_led_red_pulse_amplitude test.\n");
    
    value = rand() % 256;
    res = max30102_set_led_red_pulse_amplitude(&gs_handle, value);
    if (res)
    {
        max30102_interface_debug_print("max30102: set led red pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set led red pulse amplitude %d.\n", value);
    res = max30102_get_led_red_pulse_amplitude(&gs_handle, (uint8_t *)&value_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get led red pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check led red pulse amplitude %s.\n", value_check == value ? "ok" : "error");
    
    /* max30102_set_led_ir_pulse_amplitude/max30102_get_led_ir_pulse_amplitude test */
    max30102_interface_debug_print("max30102: max30102_set_led_ir_pulse_amplitude/max30102_get_led_ir_pulse_amplitude test.\n");
    
    value = rand() % 256;
    res = max30102_set_led_ir_pulse_amplitude(&gs_handle, value);
    if (res)
    {
        max30102_interface_debug_print("max30102: set led ir pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set led ir pulse amplitude %d.\n", value);
    res = max30102_get_led_ir_pulse_amplitude(&gs_handle, (uint8_t *)&value_check);
    if (res)
    {
        max30102_interface_debug_print("max30102: get led ir pulse amplitude failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check led ir pulse amplitude %s.\n", value_check == value ? "ok" : "error");
    
    /* max30102_set_slot/max30102_get_slot test */
    max30102_interface_debug_print("max30102: max30102_set_slot/max30102_get_slot test.\n");
    
    /* slot1 led none */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_1, MAX30102_LED_NONE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot1 led none.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_1, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot1 %s.\n", led == MAX30102_LED_NONE ? "ok" : "error");
    
    /* slot1 led red */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_1, MAX30102_LED_RED);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot1 led red.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_1, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot1 %s.\n", led == MAX30102_LED_RED ? "ok" : "error");
    
    /* slot1 led ir */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_1, MAX30102_LED_IR);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot1 led ir.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_1, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot1 %s.\n", led == MAX30102_LED_IR ? "ok" : "error");
    
    /* slot2 led none */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_2, MAX30102_LED_NONE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot2 led none.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_2, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot2 %s.\n", led == MAX30102_LED_NONE ? "ok" : "error");
    
    /* slot2 led red */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_2, MAX30102_LED_RED);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot2 led red.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_2, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot2 %s.\n", led == MAX30102_LED_RED ? "ok" : "error");
    
    /* slot2 led ir */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_2, MAX30102_LED_IR);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot2 led ir.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_2, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot2 %s.\n", led == MAX30102_LED_IR ? "ok" : "error");
    
    /* slot3 led none */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_3, MAX30102_LED_NONE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot3 led none.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_3, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot3 %s.\n", led == MAX30102_LED_NONE ? "ok" : "error");
    
    /* slot3 led red */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_3, MAX30102_LED_RED);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot3 led red.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_3, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot3 %s.\n", led == MAX30102_LED_RED ? "ok" : "error");
    
    /* slot3 led ir */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_3, MAX30102_LED_IR);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot3 led ir.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_3, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot3 %s.\n", led == MAX30102_LED_IR ? "ok" : "error");
    
    /* slot4 led none */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_4, MAX30102_LED_NONE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot4 led none.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_4, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot4 %s.\n", led == MAX30102_LED_NONE ? "ok" : "error");
    
    /* slot4 led red */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_4, MAX30102_LED_RED);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot4 led red.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_4, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot4 %s.\n", led == MAX30102_LED_RED ? "ok" : "error");
    
    /* slot4 led ir */
    res = max30102_set_slot(&gs_handle, MAX30102_SLOT_4, MAX30102_LED_IR);
    if (res)
    {
        max30102_interface_debug_print("max30102: set slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: set slot4 led ir.\n");
    res = max30102_get_slot(&gs_handle, MAX30102_SLOT_4, &led);
    if (res)
    {
        max30102_interface_debug_print("max30102: get slot failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check slot4 %s.\n", led == MAX30102_LED_IR ? "ok" : "error");
    
    /* max30102_set_die_temperature/max30102_get_die_temperature test */
    max30102_interface_debug_print("max30102: max30102_set_die_temperature/max30102_get_die_temperature test.\n");
    
    /* disable */
    res = max30102_set_die_temperature(&gs_handle, MAX30102_BOOL_FALSE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set die temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: disable die temperature.\n");
    res = max30102_get_die_temperature(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get die temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check die temperature %s.\n", enable == MAX30102_BOOL_FALSE ? "ok" : "error");
    
    /* enable */
    res = max30102_set_die_temperature(&gs_handle, MAX30102_BOOL_TRUE);
    if (res)
    {
        max30102_interface_debug_print("max30102: set die temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: enable die temperature.\n");
    res = max30102_get_die_temperature(&gs_handle, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get die temperature failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check die temperature %s.\n", res == 0 ? "ok" : "error");
    
    /* max30102_get_id test */
    max30102_interface_debug_print("max30102: max30102_get_id test.\n");

    /* get id */
    res = max30102_get_id(&gs_handle, (uint8_t *)&revision_id, (uint8_t *)&part_id);
    if (res)
    {
        max30102_interface_debug_print("max30102: get id failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: revision id is 0x%02X part id is 0x%02X.\n", revision_id, part_id);
    
    /* max30102_get_interrupt_status test */
    max30102_interface_debug_print("max30102: max30102_get_interrupt_status test.\n");
    
    /* get fifo full */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_FIFO_FULL, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: interrupt status fifo full is %d.\n", enable);
    
    /* get ppg ready */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_PPG_RDY, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: interrupt status ppg ready is %d.\n", enable);
    
    /* get alc ovf */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_ALC_OVF, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: interrupt status alc ovf is %d.\n", enable);
    
    /* get pwr ready */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_PWR_RDY, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: interrupt status pwr ready is %d.\n", enable);
    
    /* get die temp ready */
    res = max30102_get_interrupt_status(&gs_handle, MAX30102_INTERRUPT_STATUS_DIE_TEMP_RDY, &enable);
    if (res)
    {
        max30102_interface_debug_print("max30102: get interrupt status failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: interrupt status die temp ready is %d.\n", enable);
    
    /* max30102_reset test */
    max30102_interface_debug_print("max30102: max30102_reset test.\n");
    
    /* reset */
    res = max30102_reset(&gs_handle);
    if (res)
    {
        max30102_interface_debug_print("max30102: reset failed.\n");
        max30102_deinit(&gs_handle);
       
        return 1;
    }
    max30102_interface_debug_print("max30102: check reset %s.\n", res == 0 ? "ok" : "error");
    max30102_interface_delay_ms(100);
    
    /* finish register test */
    max30102_interface_debug_print("max30102: finish register test.\n");
    max30102_deinit(&gs_handle);
    
    return 0;
}
