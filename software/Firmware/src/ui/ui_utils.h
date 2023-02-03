#ifndef _UI_UTILS_H_
#define _UI_UTILS_H_

#include "lvgl.h"
#include "misc.h"

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
void update_label_cb<Temperature>(lv_event_t *e);

#endif