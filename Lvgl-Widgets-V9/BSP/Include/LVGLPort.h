#ifndef __LVGL_PORT_H__
#define __LVGL_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#if USE_OSS_LVGL_DEMO
#include "lv_demos.h"
#endif

#include "DisplayBSP.h"
#include "TouchBSP.h"
#include "SDRAMBSP.h"
#if USE_COMPONENT_GAUPORT
#include "GAUPort.h"
#endif

#if USE_RTOS_FREERTOS
#include "rtos-freertos.h"
#endif

// LVGL API Compatibility
#if LVGL_VERSION_MAJOR == 9
#include "src/lvgl_private.h"

#define lvglIsDirectMode(disp) (disp->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT)

#define lvBufAct(disp) disp->buf_act->data

#define lvBufActGotoXY(disp, x, y) lv_draw_buf_goto_xy(disp->buf_act, x, y)

#elif LVGL_VERSION_MAJOR == 8
#define lv_display_t lv_disp_t
#define lv_free      lv_mem_free

#define lvglIsDirectMode(disp) disp->driver->direct_mode

#define lvBufAct(disp)             disp->driver->draw_buf->buf_act
#define lvBufActGotoXY(disp, x, y) (lvBufAct(disp) + (x * disp->driver->hor_res + y) * (LV_COLOR_DEPTH >> 3))

#define lv_display_flush_is_last(disp) lv_disp_flush_is_last(disp->driver)
#define lv_display_flush_ready(disp)   lv_disp_flush_ready(disp->driver)

#define lv_display_set_rotation lv_disp_set_rotation
#define lv_display_get_rotation lv_disp_get_rotation

#define lv_display_get_horizontal_resolution lv_disp_get_hor_res
#define lv_display_get_vertical_resolution   lv_disp_get_ver_res

#define lv_ll_ins_tail _lv_ll_ins_tail
#define lv_ll_is_empty _lv_ll_is_empty
#define lv_ll_get_head _lv_ll_get_head
#define lv_ll_get_next _lv_ll_get_next
#define lv_ll_ins_prev _lv_ll_ins_prev
#define lv_ll_remove   _lv_ll_remove
#define lv_ll_clear    _lv_ll_clear

#define lv_area_diff      _lv_area_diff
#define lv_area_intersect _lv_area_intersect

#define LV_DISPLAY_ROTATION_0   LV_DISP_ROT_NONE
#define LV_DISPLAY_ROTATION_90  LV_DISP_ROT_90
#define LV_DISPLAY_ROTATION_180 LV_DISP_ROT_180
#define LV_DISPLAY_ROTATION_270 LV_DISP_ROT_270
#endif

extern void LVGLSetup(void);

#ifdef __cplusplus
}
#endif

#endif
