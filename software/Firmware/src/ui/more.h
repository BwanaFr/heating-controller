#ifndef _MORE_UI_H
#define _MORE_UI_H

#include "lvgl.h"
#include "ui_utils.h"

extern lv_obj_t * ui_More;

void ui_More_screen_init(void);

template <typename T>
lv_obj_t * create_info_display(lv_obj_t * parent, const char* text, uint32_t msg_id)
{
    lv_obj_t * ret = lv_label_create(parent);
    lv_obj_set_width(ret, LV_PCT(100));   /// 1
    lv_obj_set_height(ret, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ret, text);
    lv_obj_add_event_cb(ret, update_label_cb<T>, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(msg_id, ret, (void *)text);
    return ret;
}

#endif
