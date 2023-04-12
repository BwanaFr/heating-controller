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
void update_label_cb<double>(lv_event_t *e){
    lv_obj_t *label = lv_event_get_target(e);
    lv_msg_t *m = lv_event_get_msg(e);

    const char *fmt = (const char *)lv_msg_get_user_data(m);
    const double *v = (const double *)lv_msg_get_payload(m);
    if(v == nullptr){
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }else{
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        if(std::isinf(*v) || std::isnan(*v)){
            lv_label_set_text(label, "---");
        }else{
            lv_label_set_text_fmt(label, fmt, *v);
        }
    }
}

lv_obj_t * create_labeled_display(lv_obj_t * parent, const char* text, lv_obj_t*(*constructor)(lv_obj_t *))
{
    lv_obj_t * ret = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(ret, 0, LV_PART_MAIN);
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
    lv_obj_set_style_bg_opa(txtLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(txtLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(txtLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(txtLabel, 0, LV_PART_MAIN);

    return constructor(ret);
}
