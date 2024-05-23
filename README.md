# Cronus

## Set up

1. Install and [set up ESP-IDF].
2. `git clone --recurse-submodules https://github.com/ashep/cronus.git`
3. `cd cronus`

## Configure

1. `idf.py menuconfig`
2. Go to `Component config -> Cronus`, make necessary changes.
3. Exit and save changes.

## Build

```shell
idf.py build
```

## Flash

```shell
idf.py 
```

## Update dependencies

```shell
git submodule update --recursive --remote
idf.py reconfigure
```

## Changelog

## Authors

- [Oleksandr Shepetko](https://shepetko.com): initial work.

[set up ESP-IDF]: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation
