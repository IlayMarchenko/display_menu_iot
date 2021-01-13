#include <images.h>
#include <images.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/drivers/apps/Button.h>

/* Driver Configuration */
#include "ti_drivers_config.h"
#include "images.h"

void draw_image(Display_Handle hLcd, tImage image, uint8_t time);
void button_left_callback(Button_Handle handle, Button_EventMask events);
void button_right_callback(Button_Handle handle, Button_EventMask events);

Display_Handle LCD_handle;
Display_Params display_params;

uint8_t current_position = 1;
uint8_t need_to_change = 0;
uint8_t in_progress = 0;

void *mainThread(void *arg0) {

    GPIO_init();
    Button_init();
    Display_init();

    Button_Params button_params;
    Button_Params_init(&button_params);
    Button_open(BUTTON_LEFT, button_left_callback, &button_params);
    Button_open(BUTTON_RIGHT, button_right_callback, &button_params);

    Display_Params_init(&display_params);
    display_params.lineClearMode = DISPLAY_CLEAR_NONE;
    LCD_handle = Display_open(Display_Type_LCD, &display_params);
    if (LCD_handle == NULL) {
        GPIO_write(RED_LED, CONFIG_GPIO_LED_ON);
        while (1) {}
    }

    GPIO_write(GREEN_LED, CONFIG_GPIO_LED_ON);
    draw_image(LCD_handle, menu_animation_1, 1);

    while (1) {
        if (need_to_change) {
            if (in_progress) {
                switch (current_position) {
                case 1:
                    draw_image(LCD_handle, number_1, 0);
                    break;
                case 2:
                    draw_image(LCD_handle, number_2, 0);
                    break;
                case 3:
                    draw_image(LCD_handle, number_3, 0);
                    break;
                case 4:
                    draw_image(LCD_handle, number_4, 0);
                    break;
                default:
                    Display_clear(LCD_handle);
                    Display_printf(LCD_handle, 0, 0, "404");
                }
                need_to_change = 0;
            }
            else {
                switch (current_position) {
                case 1:
                    draw_image(LCD_handle, menu_animation_1, 0);
                    break;
                case 2:
                    draw_image(LCD_handle, menu_animation_2, 0);
                    break;
                case 3:
                    draw_image(LCD_handle, menu_animation_3, 0);
                    break;
                case 4:
                    draw_image(LCD_handle, menu_animation_4, 0);
                    break;
                default:
                    Display_clear(LCD_handle);
                    Display_printf(LCD_handle, 0, 0, "404");
                }
                need_to_change = 0;
            }
        }
    }
}

void draw_image(Display_Handle hLcd, tImage image, uint8_t time) {
    /* Check if the selected Display type was found and successfully opened */
    if (hLcd) {
        Graphics_Context *context = DisplayExt_getGraphicsContext(hLcd);
        if (context) {
            int splashX = (Graphics_getDisplayWidth(context) - Graphics_getImageWidth(&image)) / 2;
            int splashY = (Graphics_getDisplayHeight(context) - Graphics_getImageHeight(&image)) / 2;
            Graphics_drawImage(context, &image, splashX, splashY);
            Graphics_flushBuffer(context);
        }
        else {
            /* Not all displays have a GraphicsLib back-end */
            Display_printf(hLcd, 0, 0, "Display driver");
            Display_printf(hLcd, 1, 0, "is not");
            Display_printf(hLcd, 2, 0, "GrLib capable!");
        }
        sleep(time);
    }
}

void button_right_callback(Button_Handle handle, Button_EventMask events) {
    /* One click case */
    if (Button_EV_CLICKED == (events & Button_EV_CLICKED)) {
        if (!in_progress) {
            current_position++;
            if (current_position > 4)
                current_position = 1;
            need_to_change = 1;
        }
    }
    /* Double click case */
    if (Button_EV_DOUBLECLICKED == (events & Button_EV_DOUBLECLICKED)) {
        if (in_progress == 0) {
            in_progress = 1;
            need_to_change = 1;
        }
    }
}

void button_left_callback(Button_Handle handle, Button_EventMask events) {
    /* One click case */
    if (Button_EV_CLICKED == (events & Button_EV_CLICKED)) {
        if (!in_progress) {
            current_position--;
            if (current_position < 1)
                current_position = 4;
            need_to_change = 1;
        }
    }
    /* Double click case */
    if (Button_EV_DOUBLECLICKED == (events & Button_EV_DOUBLECLICKED)) {
        if (in_progress == 1) {
            in_progress = 0;
            need_to_change = 1;
        }
    }
}
