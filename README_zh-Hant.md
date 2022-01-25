[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md)

<div align=center>
<img src="/doc/image/logo.png"/>
</div>

## LibDriver MAX30102

[![API](https://img.shields.io/badge/api-reference-blue)](https://www.libdriver.com/docs/max30102/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

MAX30102是一個集成的脈搏血氧儀和心率監測儀生物傳感器的模塊。它集成了多個LED、光電檢測器、光器件，以及帶環境光抑制的低噪聲電子電路。 MAX30102提供完備的系統方案，使移動及可穿戴設備的設計過程變得輕鬆。 MAX30102採用一個1.8V電源和一個獨立的3.3V用於內部LED的電源，標準的I2C兼容的通信接口。可通過軟件關斷模塊，待機電流為零，實現電源始終維持供電狀態。 MAX30102可用於穿戴設備，健身輔助設備，智能手機和平板電腦等。

LibDriver MAX30102是LibDriver推出的MAX30102的全功能驅動，該驅動提供FIFO讀取，ID讀取等功能。

### 目錄

  - [說明](#說明)
  - [安裝](#安裝)
  - [使用](#使用)
    - [example fifo](#example-fifo)
  - [文檔](#文檔)
  - [貢獻](#貢獻)
  - [版權](#版權)
  - [聯繫我們](#聯繫我們)

### 說明

/src目錄包含了LibDriver MAX30102的源文件。

/interface目錄包含了LibDriver MAX30102與平台無關的IIC總線模板。

/test目錄包含了LibDriver MAX30102驅動測試程序，該程序可以簡單的測試芯片必要功能。

/example目錄包含了LibDriver MAX30102編程範例。

/doc目錄包含了LibDriver MAX30102離線文檔。

/datasheet目錄包含了MAX30102數據手冊。

/project目錄包含了常用Linux與單片機開發板的工程樣例。所有工程均採用shell腳本作為調試方法，詳細內容可參考每個工程裡面的README.md。

### 安裝

參考/interface目錄下與平台無關的IIC總線模板，完成指定平台的IIC總線驅動。

將/src目錄，/interface目錄和/example目錄加入工程。

### 使用

#### example fifo

```C
static uint8_t gs_flag;
static uint32_t gs_raw_red[32];
static uint32_t gs_raw_ir[32];
volatile uint8_t res;
volatile uint32_t timeout;
volatile uint32_t times;
uint8_t (*g_gpio_irq)(void) = NULL;

...
    
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

...
    
/* set gpio */
g_gpio_irq = max30102_fifo_irq_handler;
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
times = 3;
gs_flag = 0;
timeout = 5000;
while (timeout)
{
    if (gs_flag)
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
    max30102_fifo_deinit();
    gpio_interrupt_deinit();
    g_gpio_irq = NULL;

    return 1;
}

max30102_fifo_deinit();
gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### 文檔

在線文檔: https://www.libdriver.com/docs/max30102/index.html

離線文檔: /doc/html/index.html

### 貢獻

請聯繫lishifenging@outlook.com

### 版權

版權 (c) 2015 - 現在 LibDriver 版權所有

MIT 許可證（MIT）

特此免費授予任何獲得本軟件副本和相關文檔文件（下稱“軟件”）的人不受限制地處置該軟件的權利，包括不受限制地使用、複製、修改、合併、發布、分發、轉授許可和/或出售該軟件副本，以及再授權被配發了本軟件的人如上的權利，須在下列條件下：

上述版權聲明和本許可聲明應包含在該軟件的所有副本或實質成分中。

本軟件是“如此”提供的，沒有任何形式的明示或暗示的保證，包括但不限於對適銷性、特定用途的適用性和不侵權的保證。在任何情況下，作者或版權持有人都不對任何索賠、損害或其他責任負責，無論這些追責來自合同、侵權或其它行為中，還是產生於、源於或有關於本軟件以及本軟件的使用或其它處置。

### 聯繫我們

請聯繫lishifenging@outlook.com