/**
 * AESPL Helper Functions
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */

#include <stdint.h>
#include <stdio.h>

void print_bin(uint32_t v, uint8_t width) {
    for (uint8_t i = width; i > 0; i--) {
        printf("%lu", (v >> (i - 1)) & 1);
    }
}