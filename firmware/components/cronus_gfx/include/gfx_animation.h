/**
 * @brief     AESPL graphics, animation
 * @author    Oleksandr Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_ANIMATION_H_
#define _AESPL_GFX_ANIMATION_H_

#include <stdio.h>

#include "gfx_buffer.h"

/**
 * Animation states
 */
typedef enum {
    AESPL_GFX_ANIM_CONTINUE,
    AESPL_GFX_ANIM_RESTART,
    AESPL_GFX_ANIM_STOP,
} aespl_gfx_anim_state_t;

/**
 * Animation callback.
 */
typedef aespl_gfx_anim_state_t (*aespl_gfx_animator_t)(void *args,
                                                       uint32_t frame_n);

/**
 * Animation structure
 */
typedef struct {
    aespl_gfx_animator_t animator;
    void *args;
    uint8_t fps;
    uint32_t frame_n;
    aespl_gfx_anim_state_t state;

} aespl_gfx_animation_t;

/**
 * @brief Starts animation.
 *
 * @param fn   Animation callback.
 * @param args Callback arguments.
 * @param fps  Frames per second.
 *
 * @return Animation state
 */
aespl_gfx_animation_t *aespl_gfx_animate(aespl_gfx_animator_t fn, void *args,
                                         uint8_t fps);

#endif
