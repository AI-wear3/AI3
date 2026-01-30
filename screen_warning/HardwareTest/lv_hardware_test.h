#ifndef LV_HARDWARE_TEST_H
#define LV_HARDWARE_TEST_H

#include <lvgl.h>

// Function to display the warning screen
void lv_hardware_test(void)
{
    // Force Backlight ON (Just in case)
    pinMode(D6, OUTPUT);
    digitalWrite(D6, HIGH);

    // 1. Set background to BLUE (to verify display is working)
    // If you see Blue, the screen is on. If Black, backlight might be off.
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0000FF), LV_PART_MAIN);
    // Ensure the background is opaque
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    // 2. Create a label for text
    lv_obj_t * label = lv_label_create(scr);

    // Text: WARNING!
    lv_label_set_text(label, "WARNING!"); 

    // 3. Set text color to White
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    
    // Increase font size if possible (defaults are small)
    // lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0); // Example if font enabled

    // 4. Center the label
    lv_obj_center(label);
}

#endif
