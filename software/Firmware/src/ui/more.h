#ifndef _MORE_UI_H
#define _MORE_UI_H

#include "lvgl.h"
#include "ui_utils.h"

extern lv_obj_t * ui_More;

void ui_More_screen_init(void);

template <typename T>
lv_obj_t * create_info_display(lv_obj_t * parent, const char* text, const char* format, uint32_t msg_id)
{
    lv_obj_t * ret = lv_obj_create(parent);
    lv_obj_set_local_style_prop_meta(ret, LV_STYLE_BG_OPA, LV_STYLE_PROP_META_INITIAL, LV_PART_MAIN);
    lv_obj_set_style_border_opa(ret, 0, LV_PART_MAIN);
    lv_obj_set_width(ret, LV_PCT(100));
    lv_obj_set_height(ret, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ret, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(ret, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(ret, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(ret, 0, LV_PART_MAIN);
    
    lv_obj_t * txtLabel = lv_label_create(ret);
    lv_obj_set_width(txtLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(txtLabel, LV_SIZE_CONTENT);
    lv_label_set_text(txtLabel, text);

    lv_obj_t * valLabel = lv_label_create(ret);
    lv_obj_set_width(valLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(valLabel, LV_SIZE_CONTENT);
    lv_label_set_text(valLabel, "???");
    lv_obj_add_event_cb(valLabel, update_label_cb<T>, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(msg_id, valLabel, (void *)format);
    return ret;
}

#endif
