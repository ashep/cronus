# Cronus Firmware

## How to build

1. Install and [set up ESP-IDF].
2. `idf.py menuconfig`
3. Go to `Component config -> Cronus`, make necessary changes.
4. Exit saving changes.
5. `idf.py build`
6. `idf.py flash`

[set up ESP-IDF]: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation
