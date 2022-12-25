### 1. Board

#### 1.1 Board Info

Chip Name: Raspberry Pi 4B.

IIC Pin: SCL/SDA GPIO3/GPIO2.

GPIO Pin: INT GPIO17.

### 2. Install

#### 2.1 Dependencies

Install the necessary dependencies.

```shell
sudo apt-get install libgpiod-dev pkg-config cmake -y
```

#### 2.2 Makefile

Build the project.

```shell
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

#### 2.3 CMake

Build the project.

```shell
mkdir build && cd build 
cmake .. 
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

Test the project and this is optional.

```shell
make test
```

Find the compiled library in CMake. 

```cmake
find_package(max30102 REQUIRED)
```

#### 2.4 INT Pin

INT pin need a 4.3KΩ resistor connect to 5V.

### 3. MAX30102

#### 3.1 Command Instruction

1. show max30102 chip and driver information.

   ```shell
   max30102 (-i | --information)
   ```

2. Show max30102 help.

   ```shell
   max30102 (-h | --help)
   ```

3. Show max30102 pin connections of the current board.

   ```shell
   max30102 (-p | --port)
   ```

4. Run max30102 register test.

   ```shell
   max30102 (-t reg | --test=reg)
   ```

5. Run max30102 fifo test, times means test times.

   ```shell
   max30102 (-t fifo | --test=fifo) [--times=<num>]
   ```

6. Run max30102 fifo function, times means read times.

   ```shell
   max30102 (-e fifo | --example=fifo) [--times=<num>] 
   ```

#### 3.2 Command Example

```shell
./max30102 -i

max30102: chip is Maxim Integrated MAX30102.
max30102: manufacturer is Maxim Integrated.
max30102: interface is IIC.
max30102: driver version is 1.0.
max30102: min supply voltage is 1.7V.
max30102: max supply voltage is 2.0V.
max30102: max current is 20.00mA.
max30102: max temperature is 85.0C.
max30102: min temperature is -40.0C.
```

```shell
./max30102 -p

max30102: SCL connected to GPIO3(BCM).
max30102: SDA connected to GPIO2(BCM).
max30102: INT connected to GPIO17(BCM).
```

```shell
./max30102 -t reg

max30102: chip is Maxim Integrated MAX30102.
max30102: manufacturer is Maxim Integrated.
max30102: interface is IIC.
max30102: driver version is 1.0.
max30102: min supply voltage is 1.7V.
max30102: max supply voltage is 2.0V.
max30102: max current is 20.00mA.
max30102: max temperature is 85.0C.
max30102: min temperature is -40.0C.
max30102: start register test.
max30102: max30102_set_interrupt/max30102_get_interrupt test.
max30102: enable fifo full.
max30102: check interrupt ok.
max30102: disable fifo full.
max30102: check interrupt ok.
max30102: enable ppg ready.
max30102: check interrupt ok.
max30102: disable ppg ready.
max30102: check interrupt ok.
max30102: enable alc ovf.
max30102: check interrupt ok.
max30102: disable alc ovf.
max30102: check interrupt ok.
max30102: enable die temp ready.
max30102: check interrupt ok.
max30102: disable die temp ready.
max30102: check interrupt ok.
max30102: max30102_set_fifo_write_pointer/max30102_get_fifo_write_pointer test.
max30102: set fifo write pointer 10.
max30102: max30102_set_fifo_overflow_counter/max30102_get_fifo_overflow_counter test.
max30102: set fifo overflow counter 5.
max30102: max30102_set_fifo_read_pointer/max30102_get_fifo_read_pointer test.
max30102: set fifo read pointer 3.
max30102: max30102_set_fifo_data/max30102_get_fifo_data test.
max30102: set fifo data 7.
max30102: max30102_set_fifo_sample_averaging/max30102_get_fifo_sample_averaging test.
max30102: set sample averaging 1.
max30102: check sample ok.
max30102: set sample averaging 2.
max30102: check sample ok.
max30102: set sample averaging 4.
max30102: check sample ok.
max30102: set sample averaging 8.
max30102: check sample ok.
max30102: set sample averaging 16.
max30102: check sample ok.
max30102: set sample averaging 32.
max30102: check sample ok.
max30102: max30102_set_fifo_roll/max30102_get_fifo_roll test.
max30102: enable fifo roll.
max30102: check roll ok.
max30102: disable fifo roll.
max30102: check roll ok.
max30102: max30102_set_fifo_almost_full/max30102_get_fifo_almost_full test.
max30102: set fifo almost full 8.
max30102: check fifo almost full ok.
max30102: max30102_set_shutdown/max30102_get_shutdown test.
max30102: enable shutdown.
max30102: check shutdown ok.
max30102: disable shutdown.
max30102: check shutdown ok.
max30102: max30102_set_mode/max30102_get_mode test.
max30102: set heart rate mode.
max30102: check mode ok.
max30102: set spo2 mode.
max30102: check mode ok.
max30102: set multi led mode.
max30102: check mode ok.
max30102: max30102_set_spo2_adc_range/max30102_get_spo2_adc_range test.
max30102: set spo2 adc range 2048.
max30102: check spo2 adc range ok.
max30102: set spo2 adc range 4096.
max30102: check spo2 adc range ok.
max30102: set spo2 adc range 8192.
max30102: check spo2 adc range ok.
max30102: set spo2 adc range 16384.
max30102: check spo2 adc range ok.
max30102: max30102_set_spo2_sample_rate/max30102_get_spo2_sample_rate test.
max30102: set spo2 sample rate 50Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 100Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 200Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 400Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 800Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 1000Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 1600Hz.
max30102: check spo2 sample rate ok.
max30102: set spo2 sample rate 3200Hz.
max30102: check spo2 sample rate ok.
max30102: max30102_set_adc_resolution/max30102_get_adc_resolution test.
max30102: set adc resolution 15 bits.
max30102: check adc resolution ok.
max30102: set adc resolution 16 bits.
max30102: check adc resolution ok.
max30102: set adc resolution 17 bits.
max30102: check adc resolution ok.
max30102: set adc resolution 18 bits.
max30102: check adc resolution ok.
max30102: max30102_set_led_red_pulse_amplitude/max30102_get_led_red_pulse_amplitude test.
max30102: set led red pulse amplitude 255.
max30102: check led red pulse amplitude ok.
max30102: max30102_set_led_ir_pulse_amplitude/max30102_get_led_ir_pulse_amplitude test.
max30102: set led ir pulse amplitude 74.
max30102: check led ir pulse amplitude ok.
max30102: max30102_set_slot/max30102_get_slot test.
max30102: set slot1 led none.
max30102: check slot1 ok.
max30102: set slot1 led red.
max30102: check slot1 ok.
max30102: set slot1 led ir.
max30102: check slot1 ok.
max30102: set slot2 led none.
max30102: check slot2 ok.
max30102: set slot2 led red.
max30102: check slot2 ok.
max30102: set slot2 led ir.
max30102: check slot2 ok.
max30102: set slot3 led none.
max30102: check slot3 ok.
max30102: set slot3 led red.
max30102: check slot3 ok.
max30102: set slot3 led ir.
max30102: check slot3 ok.
max30102: set slot4 led none.
max30102: check slot4 ok.
max30102: set slot4 led red.
max30102: check slot4 ok.
max30102: set slot4 led ir.
max30102: check slot4 ok.
max30102: max30102_set_die_temperature/max30102_get_die_temperature test.
max30102: disable die temperature.
max30102: check die temperature ok.
max30102: enable die temperature.
max30102: check die temperature ok.
max30102: max30102_get_id test.
max30102: revision id is 0x03 part id is 0x15.
max30102: max30102_get_interrupt_status test.
max30102: interrupt status fifo full is 0.
max30102: interrupt status ppg ready is 0.
max30102: interrupt status alc ovf is 0.
max30102: interrupt status pwr ready is 0.
max30102: interrupt status die temp ready is 0.
max30102: max30102_reset test.
max30102: check reset ok.
max30102: finish register test.
```

```shell
./max30102 -t fifo --times=3

max30102: chip is Maxim Integrated MAX30102.
max30102: manufacturer is Maxim Integrated.
max30102: interface is IIC.
max30102: driver version is 1.0.
max30102: min supply voltage is 1.7V.
max30102: max supply voltage is 2.0V.
max30102: max current is 20.00mA.
max30102: max temperature is 85.0C.
max30102: min temperature is -40.0C.
max30102: start fifo test.
max30102: irq die temp rdy.
max30102: temperature is 28.1250C.
max30102: irq fifo full with 17.
max30102: irq fifo full with 17.
max30102: irq fifo full with 17.
max30102: finish fifo test.
```

```shell
./max30102 -e fifo --times=3

max30102: irq fifo full with 17.
max30102: 1/3.
max30102: irq fifo full with 17.
max30102: 2/3.
max30102: irq fifo full with 17.
max30102: 3/3.
```

```shell
./max30102 -h

Usage:
  max30102 (-i | --information)
  max30102 (-h | --help)
  max30102 (-p | --port)
  max30102 (-t reg | --test=reg)
  max30102 (-t fifo | --test=fifo) [--times=<num>]
  max30102 (-e fifo | --example=fifo) [--times=<num>]

Options:
  -e <fifo>, --example=<fifo>    Run the driver example.
  -h, --help                     Show the help.
  -i, --information              Show the chip information.
  -p, --port                     Display the pin connections of the current board.
  -t <reg | fifo>, --test=<reg | fifo>
                                 Run the driver test.
      --times=<num>              Set the running times.([default: 3])
```

