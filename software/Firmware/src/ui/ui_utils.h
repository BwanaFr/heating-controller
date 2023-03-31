#ifndef _UI_UTILS_H_
#define _UI_UTILS_H_

#include "lvgl.h"

/**
 * Updates label text
*/
template <typename T>
void update_label_cb(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const T *v = (const T *)lv_msg_get_payload(m);
    if(v == nullptr){
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(label, fmt, *v);
    }
}

/**
 * Specialization for the string
*/
template <>
void update_label_cb<const char *>(lv_event_t *e);

/**
 * Specialization for the Temperature
*/
template <>
void update_label_cb<double>(lv_event_t *e);

/**
    Create an info display
**/
template <typename T>
lv_obj_t * create_info_display(lv_obj_t * parent, const char* text, const char* format, uint32_t msg_id)
{
    lv_obj_t * ret = lv_obj_create(parent);
    lv_obj_set_width(ret, LV_PCT(100));
    lv_obj_set_height(ret, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ret, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ret, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(ret, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(ret, 0, LV_PART_MAIN);
    
    lv_obj_t * txtLabel = lv_label_create(ret);
    lv_obj_set_width(txtLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(txtLabel, LV_SIZE_CONTENT);
    lv_label_set_text(txtLabel, text);
    lv_obj_set_style_bg_color(txtLabel, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(txtLabel, 100, LV_PART_MAIN);
    lv_obj_set_style_pad_all(txtLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(txtLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(txtLabel, 0, LV_PART_MAIN);

    lv_obj_t * valLabel = lv_label_create(ret);
    lv_obj_set_width(valLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(valLabel, LV_SIZE_CONTENT);
    lv_label_set_text(valLabel, "???");
    lv_obj_set_style_text_color(valLabel, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
    lv_obj_set_style_pad_right(valLabel, LV_DPX(10), LV_PART_MAIN);
    // lv_obj_set_style_bg_color(valLabel, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_PART_MAIN);
    // lv_obj_set_style_bg_opa(valLabel, 100, LV_PART_MAIN);
    lv_obj_add_event_cb(valLabel, update_label_cb<T>, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(msg_id, valLabel, (void *)format);
    return ret;
}

#endif

