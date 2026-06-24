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
 * @file      driver_max30102_interface_template.c
 * @brief     driver max30102 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-11-13
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/11/13  <td>1.0      <td>Shifeng Li  <td>first upload
 * <tr><td>2022/6/26   <td>1.1      <td>Yuchao Geng <td>Add ESP32-C3 support
 * </table>
 */

#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver_max30102_interface.h"


typedef struct {
    int num;
    i2c_config_t cfg;
}i2c_t;

i2c_t *i2c = NULL;

#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t max30102_interface_iic_init(void)
{
    esp_err_t err = ESP_OK;
    uint8_t i2c_num = 0;
    uint8_t scl = 2;
    uint8_t sda = 3;
    uint32_t clk_speed = 400000;

    if(i2c_num > 1) {
        err = ESP_ERR_INVALID_ARG;
        goto error;
    }

    i2c = malloc(sizeof(i2c_t));
    if(i2c == NULL) {
        err = ESP_ERR_NO_MEM;
        goto error;
    }

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1 << scl ) | ( 1 << sda ));
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    memset(i2c,0,sizeof(i2c_t));
    i2c->num = i2c_num;
    i2c->cfg.mode = I2C_MODE_MASTER,
    i2c->cfg.sda_io_num = sda,
    i2c->cfg.sda_pullup_en = GPIO_PULLUP_ENABLE,
    i2c->cfg.scl_io_num = scl,
    i2c->cfg.scl_pullup_en = GPIO_PULLUP_ENABLE,
    i2c->cfg.master.clk_speed = clk_speed,
    // i2c->cfg.clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */

    err = i2c_param_config(i2c->num, &i2c->cfg);
    if (err != ESP_OK) {
        goto error;
    }
    err = i2c_driver_install(i2c->num, i2c->cfg.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (err != ESP_OK) {    
        goto error;   
    }
    ESP_LOGI("max30102","%s:init success!SCL:%d,SDA:%d\n",__func__,i2c->cfg.scl_io_num,i2c->cfg.sda_io_num);
    return 0;
error:
    ESP_LOGE("max30102","%s:%s\n",__func__,esp_err_to_name(err));
    if(i2c != NULL) {
        free(i2c);
        i2c = NULL;
    }
    return 1;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t max30102_interface_iic_deinit(void)
{
    i2c_driver_delete(i2c->num);
    free(i2c);
    i2c = NULL;
    return 0;
}

// External Wire.h equivalent error Codes
typedef enum {
    I2C_ERROR_OK=0,
    I2C_ERROR_DEV,
    I2C_ERROR_ACK,
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUS,
    I2C_ERROR_BUSY,
    I2C_ERROR_MEMORY,
    I2C_ERROR_CONTINUE,
    I2C_ERROR_NO_BEGIN
} i2c_err_t;

#define I2C_TRANS_BUF_MINIMUM_SIZE     (1024) /* It is required to have allocate one i2c_cmd_desc_t per command:*/


/**
 * @brief      interface iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t max30102_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    // printf("addr:0x%x,reg:%x,len:%d\n",addr,reg,len);

    esp_err_t err = ESP_OK;
    uint8_t buffer[I2C_TRANS_BUF_MINIMUM_SIZE] = { 0 };
    uint32_t ticks_to_wait = 0xFFFFFF;

    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        printf("start fail\n");
        goto end;
    }

    err = i2c_master_write_byte(handle, addr | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        printf("write addr fail\n");
        goto end;
    }

    err = i2c_master_write_byte(handle, reg, true);
    if (err != ESP_OK) {
        printf("write reg fail\n");
        goto end;
    }

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        printf("start2 fail\n");
        goto end;
    }
    
    err = i2c_master_write_byte(handle, addr | I2C_MASTER_READ, true);
    if (err != ESP_OK) {
        printf("start read fail\n");
        goto end;
    }

    err = i2c_master_read(handle, buf, len, I2C_MASTER_LAST_NACK);
    if (err != ESP_OK) {
        printf("start read data fail\n");
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(i2c->num, handle, ticks_to_wait);
    if (err != ESP_OK) {
        printf("begin fail,err:0x%x\n",err);
        goto end;
    }

end:
    i2c_cmd_link_delete_static(handle);
    return err;

}

/**
 * @brief     interface iic bus write
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t max30102_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    esp_err_t err = ESP_OK;
    uint8_t buffer[I2C_TRANS_BUF_MINIMUM_SIZE] = { 0 };
    uint32_t ticks_to_wait = 0xFFFFFF;

    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, addr | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, reg, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write(handle, buf, len, true);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(i2c->num, handle, ticks_to_wait);

end:
    i2c_cmd_link_delete_static(handle);
    return err;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void max30102_interface_delay_ms(uint32_t ms)
{
    esp_rom_delay_us(1000);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void max30102_interface_debug_print(const char *const fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    esp_log_writev(ESP_LOG_INFO, "max30102", fmt, list);
    va_end(list);
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @note      none
 */
void max30102_interface_receive_callback(uint8_t type)
{
    switch (type)
    {
        case MAX30102_INTERRUPT_STATUS_FIFO_FULL :
        {
            max30102_interface_debug_print("max30102: irq fifo full.\n");
            
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
