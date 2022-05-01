[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.png"/>
</div>

## LibDriver MAX30102

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/max30102/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

Das MAX30102 ist ein integriertes Pulsoximetrie- und Herzfrequenzüberwachungsmodul. Es umfasst interne LEDs, Fotodetektoren, optische Elemente und rauscharme Elektronik mit Umgebungslichtunterdrückung. Der MAX30102 bietet eine vollständige Systemlösung zur Vereinfachung des Design-in-Prozesses für mobile und tragbare Geräte. Der MAX30102 wird mit einer einzigen 1,8-V-Stromversorgung und einer separaten 3,3-V-Stromversorgung für die internen LEDs betrieben. Die Kommunikation erfolgt über eine standardmäßige I2C-kompatible Schnittstelle. Das Modul kann über Software ohne Standby-Strom heruntergefahren werden, sodass die Stromschienen jederzeit mit Strom versorgt werden. Es kann in tragbaren Geräten, Fitness-Assistenten, Smartphones, Tablets usw. verwendet werden.

LibDriver MAX30102 ist der voll funktionsfähige Treiber von MAX30102, der von LibDriver gestartet wurde. Es bietet Fifo-Lesen, ID-Lesen und andere Funktionen. LibDriver ist MISRA-konform.

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example fifo](#example-fifo)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver MAX30102-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver MAX30102 IIC。

/test enthält den Testcode des LibDriver MAX30102-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen。

/example enthält LibDriver MAX30102-Beispielcode.

/doc enthält das LibDriver MAX30102-Offlinedokument.

/Datenblatt enthält MAX30102-Datenblatt。

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

### Installieren

Verweisen Sie auf eine plattformunabhängige IIC-Schnittstellenvorlage und stellen Sie Ihren Plattform-IIC-Treiber fertig.

Fügen Sie /src, /interface und /example zu Ihrem Projekt hinzu.

### Nutzung

#### example fifo

```C
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

### Dokument

Online-Dokumente: https://www.libdriver.com/docs/max30102/index.html

Offline-Dokumente: /doc/html/index.html

### Beitrag

Bitte senden Sie eine E-Mail an lishifenging@outlook.com

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesentliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com