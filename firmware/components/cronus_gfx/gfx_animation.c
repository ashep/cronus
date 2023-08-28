#include "gfx_animation.h"

#include <stdbool.h>
#include <stdio.h>

#include "gfx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void animate_task(void *args) {
    aespl_gfx_animation_t *anim = (aespl_gfx_animation_t *)args;
    TickType_t delay = pdMS_TO_TICKS(1000 / anim->fps);

    for (;;) {
        if (anim->state == AESPL_GFX_ANIM_RESTART) {
            anim->frame_n = 0;
            anim->state = AESPL_GFX_ANIM_CONTINUE;
        }

        if (anim->state == AESPL_GFX_ANIM_CONTINUE) {
            anim->state = anim->animator(anim->args, anim->frame_n++);
            vTaskDelay(delay);
        }

        if (anim->state == AESPL_GFX_ANIM_STOP) {
            free(anim);
            break;
        }
    }

    vTaskDelete(NULL);
}

aespl_gfx_animation_t *aespl_gfx_animate(aespl_gfx_animator_t fn, void *args,
                                         uint8_t fps) {
    aespl_gfx_animation_t *anim = malloc(sizeof(aespl_gfx_animation_t));
    if (!anim) {
        return NULL;
    }

    anim->animator = fn;
    anim->args = args;
    anim->fps = fps;
    anim->frame_n = 0;
    anim->state = AESPL_GFX_ANIM_CONTINUE;

    xTaskCreate(animate_task, "animation", 4096, (void *)anim, 0, NULL);

    return anim;
}
