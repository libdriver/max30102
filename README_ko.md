[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver MAX30102

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/max30102/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

MAX30102는 통합 맥박 산소 측정기 및 심박수 모니터 모듈입니다. 여기에는 내부 LED, 광검출기, 광학 요소 및 주변광 제거 기능이 있는 저잡음 전자 장치가 포함됩니다. MAX30102는 모바일 및 웨어러블 기기를 위한 설계 프로세스를 용이하게 하는 완벽한 시스템 솔루션을 제공합니다. MAX30102는 단일 1.8V 전원 공급 장치와 내부 LED를 위한 별도의 3.3V 전원 공급 장치에서 작동합니다. 통신은 표준 I2C 호환 인터페이스를 통해 이루어집니다. 이 모듈은 대기 전류가 0인 소프트웨어를 통해 종료할 수 있으므로 전원 레일에 항상 전원이 공급된 상태를 유지할 수 있습니다. 웨어러블 장치, 피트니스 보조 장치, 스마트폰, 태블릿 등에 사용할 수 있습니다.

LibDriver MAX30102는 LibDriver에서 출시한 MAX30102의 전체 기능 드라이버입니다. 그것은 fifo 읽기, id 읽기 및 기타 기능을 제공합니다. LibDriver는 MISRA를 준수합니다.

### 콘텐츠

  - [설명](#설명)
  - [설치](#설치)
  - [사용](#사용)
    - [example fifo](#example-fifo)
  - [문서](#문서)
  - [기고](#기고)
  - [저작권](#저작권)
  - [문의하기](#문의하기)

### 설명

/src 디렉토리에는 LibDriver MAX30102의 소스 파일이 포함되어 있습니다.

/interface 디렉토리에는 LibDriver MAX30102용 플랫폼 독립적인 IIC버스 템플릿이 포함되어 있습니다.

/test 디렉토리에는 LibDriver MAX30102드라이버 테스트 프로그램이 포함되어 있어 칩의 필요한 기능을 간단히 테스트할 수 있습니다.

/example 디렉토리에는 LibDriver MAX30102프로그래밍 예제가 포함되어 있습니다.

/doc 디렉토리에는 LibDriver MAX30102오프라인 문서가 포함되어 있습니다.

/datasheet 디렉토리에는 MAX30102데이터시트가 있습니다.

/project 디렉토리에는 일반적으로 사용되는 Linux 및 마이크로컨트롤러 개발 보드의 프로젝트 샘플이 포함되어 있습니다. 모든 프로젝트는 디버깅 방법으로 셸 스크립트를 사용하며, 자세한 내용은 각 프로젝트의 README.md를 참조하십시오.

/misra 에는 LibDriver misra 코드 검색 결과가 포함됩니다.

### 설치

/interface 디렉토리에서 플랫폼 독립적인IIC 버스 템플릿을 참조하여 지정된 플랫폼에 대한 IIC버스 드라이버를 완성하십시오.

/src 디렉토리, /interface 디렉토리 및 /example 디렉토리를 프로젝트에 추가하십시오.

### 사용

/example 디렉터리의 예제를 참조하여 자신만의 드라이버를 완성할 수 있습니다. 기본 프로그래밍 예제를 사용하려는 경우 사용 방법은 다음과 같습니다.

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

### 문서

온라인 문서: [https://www.libdriver.com/docs/max30102/index.html](https://www.libdriver.com/docs/max30102/index.html).

오프라인 문서: /doc/html/index.html.

### 기고

CONTRIBUTING.md 를 참조하십시오.

### 저작권

저작권 (c) 2015 - 지금 LibDriver 판권 소유

MIT 라이선스(MIT)

이 소프트웨어 및 관련 문서 파일("소프트웨어")의 사본을 얻은 모든 사람은 이에 따라 무제한 사용, 복제, 수정, 통합, 출판, 배포, 2차 라이선스를 포함하여 소프트웨어를 처분할 수 있는 권리가 부여됩니다. 소프트웨어의 사본에 대한 라이선스 및/또는 판매, 그리고 소프트웨어가 위와 같이 배포된 사람의 권리에 대한 2차 라이선스는 다음 조건에 따릅니다.

위의 저작권 표시 및 이 허가 표시는 이 소프트웨어의 모든 사본 또는 내용에 포함됩니다.

이 소프트웨어는 상품성, 특정 목적에의 적합성 및 비침해에 대한 보증을 포함하되 이에 국한되지 않는 어떠한 종류의 명시적 또는 묵시적 보증 없이 "있는 그대로" 제공됩니다. 어떤 경우에도 저자 또는 저작권 소유자는 계약, 불법 행위 또는 기타 방식에 관계없이 소프트웨어 및 기타 소프트웨어 사용으로 인해 발생하거나 이와 관련하여 발생하는 청구, 손해 또는 기타 책임에 대해 책임을 지지 않습니다.

### 문의하기

연락주세요lishifenging@outlook.com.