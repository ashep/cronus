# Cronus

**Cronus** is a minimalistic digital clock built on ESP32 MCU.

![showcase-03](img/showcase/03.jpg)

<details>
<summary>See more samples</summary>

![showcase-01](img/showcase/01.jpg)
![showcase-02](img/showcase/02.jpg)
![showcase-04](img/showcase/04.jpg)
![showcase-05](img/showcase/05.jpg)
</details>

## Circuit Diagram

- KiKad sources are in the [schematic](schematic) directory.
- A PDF version is in the [schematic/pdf/Cronus.pdf](schematic/pdf/Cronus.pdf) file.

## Bill of Materials

### ESP32 Dev Board

You can use any other ESP32 dev board with enough flash memory to hold the firmware and OTA updates.

I used a 44-pin double type-C ESP32-S3 board since I had it in my storage.
You can [find](https://www.aliexpress.com/w/wholesale-ESP32-Dual-Type-C-Development-Board.html) it on AliExpress.

![ESP32 Board](img/components/esp32board-01.jpg)

### Display

Cronus uses popular MAX7219-driven 8x8 LED matrices, which can be easily found
on [AliExpress](https://www.aliexpress.com/w/wholesale-MAX7219-Dot-Matrix-Module.html).
**Be careful about board size** since two versions exist in the wild: the 128mm long and the 132mm.
You need the first one because the 132mm version has 1mm ugly gaps between matrices.

![Display board](img/components/displayboard-01.jpg)

### DS3231 RTC Dev Board

Cronus optionally relies on an external DS3231-based real-time clock to keep time running even when the power is
disconnected. I used an inexpensive and convenient dev board
from [AliExpress](https://www.aliexpress.com/w/wholesale-DS3231-module.html).

![RTC Board](img/components/rtcboard-01.jpg)
![RTC Board](img/components/rtcboard-02.jpg)

### Motherboard

If you use the dev boards mentioned above, you can connect them using wires or using “motherboard”, which speeds up the
assembly process and makes connections more reliable.

In the [schematic/gerber](schematic/gerber) directory, you can find ready to use Gerber files that you could send to
your favourite PCB manufacturer like JLC PCB or PCB Way.

![Motherboard ](img/components/mboard-01.jpg)

The PCB is simple and single-sided, so the manufacturing process is easy.
In the [schematic/gcode](schematic/gcode) directory, you can find G-code files suitable for producing homemade PCBs
using a CNC router.

The files are generated from the [gerber](schematic/gerber) sources by [genpcb.sh](schematic/genpcb.sh) script, which
uses the [pcb2gcode](https://github.com/pcb2gcode/pcb2gcode) tool.

![Motherboard](img/components/mboard-02.jpg)

### Enclosure

3D models and the front-glass cutout CNC program are in the [enclosure](enclosure) directory. 
The CNC program is made for cutting 2mm acrylic glass with a 1mm flute bit.

## Building

### Display

Use two display boards, 3D-printed mounting brackets, and M3 screws and nuts to assemble the entire display.

![Display assembly](img/assembly/display-01.jpg)
![Display assembly](img/assembly/display-02.jpg)
![Display assembly](img/assembly/display-03.jpg)
![Display assembly](img/assembly/display-04.jpg)

Connect the output of the bottom module to the top's input one, and connect power lines.

![Display assembly](img/assembly/display-05.jpg)
![Display assembly](img/assembly/display-06.jpg)

Put matrices in their places.

![Display assembly](img/assembly/display-07.jpg)
![Display assembly](img/assembly/display-08.jpg)

### Motherboard

Put and solder dev modules in the motherboard.
Solder SMD components.
Attach and solder wires.

![Motherboard assembly](img/assembly/motherboard-01.jpg)
![Motherboard assembly](img/assembly/motherboard-02.jpg)

Mount the motherboard behind the display.

![Motherboard assembly](img/assembly/motherboard-03.jpg)

Connect display to the motherboard.

![Motherboard assembly](img/assembly/motherboard-04.jpg)

Connect display and motherboard power lines to the USB connector.

![Motherboard assembly](img/assembly/motherboard-05.jpg)

Flash firmware, connect power to the USB connector, and check whether everything works correctly.

![Motherboard assembly](img/assembly/motherboard-06.jpg)
![Motherboard assembly](img/assembly/motherboard-07.jpg)

### Enclosure

To do.

## Flashing Firmware

To do.

## Development

Clone the repo:

```shell
git clone --recurse-submodules https://github.com/ashep/cronus.git
```

Or update an already cloned one to the latest version:

```shell
git pull
git submodule update --recursive --remote
```

## Changelog

### 0.0.1

First release.

## Authors

- [Oleksandr Shepetko](https://shepetko.com) -- initial work.
