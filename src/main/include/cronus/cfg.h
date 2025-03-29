#pragma once

// Display 1
#define CRONUS_CFG_ID_DISPLAY_0_TYPE    30
#define CRONUS_CFG_ID_DISPLAY_0_FLAGS   31
#define CRONUS_CFG_ID_DISPLAY_0_PIN_CS  32 // chip select/enable
#define CRONUS_CFG_ID_DISPLAY_0_PIN_CLK 33 // clock
#define CRONUS_CFG_ID_DISPLAY_0_PIN_RS  34 // data/instructions
#define CRONUS_CFG_ID_DISPLAY_0_PIN_Rw  35 // read/write
#define CRONUS_CFG_ID_DISPLAY_0_PIN_RST 36 // reset
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D0  37 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D1  38 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D2  39 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D3  40 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D4  41 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D5  42 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D6  43 // data
#define CRONUS_CFG_ID_DISPLAY_0_PIN_D7  44 // data
#define CRONUS_CFG_ID_DISPLAY_0_BRI_MAX 45 // brightness hard limit

// Display 1 flags
#define CRONUS_CFG_FLAG_DISPLAY_0_REVERSE 0 // is multi-device display input connected to the last device

// Display 2
#define CRONUS_CFG_ID_DISPLAY_1_TYPE    50
#define CRONUS_CFG_ID_DISPLAY_1_FLAGS   51
#define CRONUS_CFG_ID_DISPLAY_1_PIN_CS  52 // chip select/enable
#define CRONUS_CFG_ID_DISPLAY_1_PIN_CLK 53 // clock
#define CRONUS_CFG_ID_DISPLAY_1_PIN_RS  54 // data/instructions
#define CRONUS_CFG_ID_DISPLAY_1_PIN_Rw  55 // read/write
#define CRONUS_CFG_ID_DISPLAY_1_PIN_RST 56 // reset
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D0  57 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D1  58 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D2  59 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D3  60 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D4  61 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D5  62 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D6  63 // data
#define CRONUS_CFG_ID_DISPLAY_1_PIN_D7  64 // data
#define CRONUS_CFG_ID_DISPLAY_1_BRI_MAX 65 // brightness hard limit

// Display 2 flags
#define CRONUS_CFG_FLAG_DISPLAY_1_REVERSE 0 // is multi-device display input connected to the last device

// Peripheral devices
#define CRONUS_CFG_ID_PRP_RTC_PIN_SCL 70 // RTC clock
#define CRONUS_CFG_ID_PRP_RTC_PIN_SDA 71 // RTC data

// Starting after this ID can be changed by users
#define CRONUS_CFG_ID_USER_START 127

// Various user settings
#define CRONUS_CFG_ID_USER_SHOW_MODE         128 // widget show mode, see CRONUS_CFG_USER_SHOW_MODE_XXX
#define CRONUS_CFG_ID_USER_BRIGHTNESS_MIN    129 // minimum brightness
#define CRONUS_CFG_ID_USER_BRIGHTNESS_MAX    130 // maximum brightness
#define CRONUS_CFG_ID_USER_SHOW_DUR_TIME     131 // current time show duration
#define CRONUS_CFG_ID_USER_SHOW_DUR_DATE     132 // current date show duration
#define CRONUS_CFG_ID_USER_SHOW_DUR_DOW      133 // day of week show duration
#define CRONUS_CFG_ID_USER_SHOW_DUR_AMB_TEMP 134 // ambient temperature show duration
#define CRONUS_CFG_ID_USER_SHOW_DUR_ODR_TEMP 135 // outdoor temperature show duration
#define CRONUS_CFG_ID_USER_ALLOW_ALPHA_UPD   136 // allow firmware upgrades to alpha versions

#define CRONUS_CFG_USER_SHOW_MODE_SINGLE_LINE 0
#define CRONUS_CFG_USER_SHOW_MODE_MULTI_LINE  1

typedef enum {
    CRONUS_CFG_DISPLAY_TYPE_NONE,
    CRONUS_CFG_DISPLAY_TYPE_MAX7219_32X16,
} cronus_cfg_display_type_t;