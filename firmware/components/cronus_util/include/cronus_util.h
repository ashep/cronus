/**
 * AESPL Common Functions
 *
 * Author: Oleksandr Shepetko <a@shepetko.com>
 * License: MIT
 */
#ifndef _AESPL_COMMON_H_
#define _AESPL_COMMON_H_

#include "stdint.h"

#define AESPL_ERR_BASE 0x1000

/**
 * @brief Prints binary representation of an integer.
 *
 * @param v      Value
 * @param width  Value width in bits
 */
void print_bin(uint32_t v, uint8_t width);

#endif
