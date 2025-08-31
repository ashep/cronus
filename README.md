# Cronus

![Banner](img/artwork/CronusBannerGitHub.jpg)

**Cronus** is a minimalistic digital clock built on top of the ESP32 MCU with the following features in mind.

- Show the current time, date, and weather conditions.
- Simple, with a bright display.
- Brightness auto-adjustment depending on the ambient light level.
- No buttons.
- Automatic Over-the-Air firmware upgrades.
- No disgusting bloody Arduino.

![Showcase](img/showcase/00-1.jpg)
<details>
<summary>See more</summary>

![Showcase](img/showcase/00-2.jpg)
![Showcase](img/showcase/01.jpg)
![Showcase](img/showcase/02.jpg)
![Showcase](img/showcase/03.jpg)
![Showcase](img/showcase/04.jpg)
![Showcase](img/showcase/05.jpg)
</details>

## How to make it

See the [Wiki](https://github.com/ashep/cronus/wiki).

## Contributing

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

### 0.0.3 (2025-08-31)

- Added [night mode](https://github.com/ashep/cronus/issues/21) for colorful screens.
- Added [custom location](https://github.com/ashep/cronus/issues/13) option.
- Fixed [Wi-Fi reconnection](https://github.com/ashep/cronus/issues/18) at startup.

### 0.0.2 (2025-08-11)

- New Bluetooth configuration interface added.
- Weather icons added.

### 0.0.1 (2025-06-21)

First stable release.

## Authors

- [Oleksandr Shepetko](https://shepetko.com).
