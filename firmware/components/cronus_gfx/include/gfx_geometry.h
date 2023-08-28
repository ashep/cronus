/**
 * @brief     AESPL graphics, geometry
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_GEOMETRY_H_
#define _AESPL_GFX_GEOMETRY_H_

#include <stdio.h>

#include "gfx.h"
#include "gfx_buffer.h"

/**
 * Line.
 */
typedef struct {
    aespl_gfx_point_t p1;
    aespl_gfx_point_t p2;
} aespl_gfx_line_t;

/**
 * Polygon.
 */
typedef struct {
    uint8_t n_corners;
    aespl_gfx_point_t *corners;
} aespl_gfx_poly_t;

/**
 * @brief Draws a line
 *
 * @param buf    Buffer.
 * @param line   Line.
 * @param color  Color.
 */
void aespl_gfx_line(aespl_gfx_buf_t *buf, const aespl_gfx_line_t *line,
                    uint32_t color);

/**
 * @brief Draws a polygon.
 *
 * @param buf    Buffer.
 * @param poly   Points.
 * @param color  Color.
 */
void aespl_gfx_poly(aespl_gfx_buf_t *buf, const aespl_gfx_poly_t *poly,
                    uint32_t color);

/**
 * @brief Draws a rectangle.
 *
 * @param buf    Buffer.
 * @param p1     Top left point.
 * @param p2     Bottom right point.
 * @param color  Color.
 */
void aespl_gfx_rect(aespl_gfx_buf_t *buf, aespl_gfx_point_t p1,
                    aespl_gfx_point_t p2, uint32_t color);

/**
 * @brief Draws a triangle.
 *
 * @param buf    Buffer.
 * @param p1     Point 1.
 * @param p2     Point 2.
 * @param p3     Point 3.
 * @param color  Color.
 */
void aespl_gfx_tri(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1,
                   const aespl_gfx_point_t p2, const aespl_gfx_point_t p3,
                   uint32_t color);

#endif
