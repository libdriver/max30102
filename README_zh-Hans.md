[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver MAX30102

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/max30102/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

MAX30102是一个集成的脉搏血氧仪和心率监测仪生物传感器的模块。它集成了多个LED、光电检测器、光器件，以及带环境光抑制的低噪声电子电路。MAX30102提供完备的系统方案，使移动及可穿戴设备的设计过程变得轻松。MAX30102采用一个1.8V电源和一个独立的3.3V用于内部LED的电源，标准的I2C兼容的通信接口。可通过软件关断模块，待机电流为零，实现电源始终维持供电状态。MAX30102可用于穿戴设备，健身辅助设备，智能手机和平板电脑等。

LibDriver MAX30102是LibDriver推出的MAX30102的全功能驱动，该驱动提供FIFO读取，ID读取等功能并且它符合MISRA标准。

### 目录

  - [说明](#说明)
  - [安装](#安装)
  - [使用](#使用)
    - [example fifo](#example-fifo)
  - [文档](#文档)
  - [贡献](#贡献)
  - [版权](#版权)
  - [联系我们](#联系我们)

### 说明

/src目录包含了LibDriver MAX30102的源文件。

/interface目录包含了LibDriver MAX30102与平台无关的IIC总线模板。

/test目录包含了LibDriver MAX30102驱动测试程序，该程序可以简单的测试芯片必要功能。

/example目录包含了LibDriver MAX30102编程范例。

/doc目录包含了LibDriver MAX30102离线文档。

/datasheet目录包含了MAX30102数据手册。

/project目录包含了常用Linux与单片机开发板的工程样例。所有工程均采用shell脚本作为调试方法，详细内容可参考每个工程里面的README.md。

### 安装

参考/interface目录下与平台无关的IIC总线模板，完成指定平台的IIC总线驱动。

将/src目录，/interface目录和/example目录加入工程。

### 使用

#### example fifo

```C
#include "driver_max30102_fifo.h"

static uint8_t gs_flag;
static uint32_t gs_raw_red[32];
static uint32_t gs_raw_ir[32];
uint8_t res;
uint32_t timeout;
uint32_t times;
uint8_t (*g_gpio_irq)(void) = NULL;

...
    
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
            max30102_interface_debug_print("max30102: unknown code.\n");
            
            break;
        }
    }
}

...
    
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
times = 3;
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
    (void)max30102_fifo_deinit();
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}

(void)max30102_fifo_deinit();
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 文档

在线文档: https://www.libdriver.com/docs/max30102/index.html

离线文档: /doc/html/index.html

### 贡献

请联系lishifenging@outlook.com

### 版权

版权 (c) 2015 - 现在 LibDriver 版权所有

MIT 许可证（MIT）

特此免费授予任何获得本软件副本和相关文档文件（下称“软件”）的人不受限制地处置该软件的权利，包括不受限制地使用、复制、修改、合并、发布、分发、转授许可和/或出售该软件副本，以及再授权被配发了本软件的人如上的权利，须在下列条件下：

上述版权声明和本许可声明应包含在该软件的所有副本或实质成分中。

本软件是“如此”提供的，没有任何形式的明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和不侵权的保证。在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论这些追责来自合同、侵权或其它行为中，还是产生于、源于或有关于本软件以及本软件的使用或其它处置。

### 联系我们

请联系lishifenging@outlook.com