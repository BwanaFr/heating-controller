#include "ui_utils.h"
#include <cmath>

template <>
void update_label_cb<const char *>(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const char *v = (const char *)lv_msg_get_payload(m);
    if(v == nullptr){
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(label, fmt, v);
    }
}

template <>
void update_label_cb<Temperature>(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const Temperature *v = (const Temperature *)lv_msg_get_payload(m);
    if(v == nullptr){
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        if(std::isinf(*v)){
            lv_label_set_text(label, "---");    
        }else{
            lv_label_set_text_fmt(label, fmt, *v);
        }
    }
}