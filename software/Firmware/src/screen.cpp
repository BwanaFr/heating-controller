#include "screen.h"

//LVGL and LCD related includes
#include "lv_conf.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "TouchLib.h"
#include "pin_config.h"
#include "Wire.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"


//LVGL and LCD objects
static esp_lcd_panel_io_handle_t io_handle = NULL; // Handle to esp_lcd
static esp_lcd_panel_handle_t panel_handle = NULL; // Handle to esp_lcd_panel
static lv_disp_draw_buf_t disp_buf;         // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;              // contains callback functions
static lv_color_t *lv_disp_buf;             // Display buffer (allocated on heap)
static bool is_initialized_lvgl = false;    // is LVGL initialized?
static unsigned long next_call = 0;         // Next tick to call lvgl

//Touch screen library
static TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS, PIN_TOUCH_RES);
static bool inited_touch = false;                  // is touch screen initialized?
static bool go_touch_int = false;                  // flag to know if we had an touch interrupt

//Global variables
static unsigned long lastTouchEvent = 0;    // Last time the screen was touched
static bool screenBlanked = false;          // True if screen is blanked
const unsigned long SCREEN_BLANK = 30000;   // Blank screen after 30 seconds
static bool systemReady = false;            // True when system is ready

/**
 * Callback to notify lvgl when flush is ready
**/
static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
  if (is_initialized_lvgl) {
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
  }
  return false;
}

/**
 * Callback from lvgl to flush display
*/
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
  esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
  int offsetx1 = area->x1;
  int offsetx2 = area->x2;
  int offsety1 = area->y1;
  int offsety2 = area->y2;
  // copy a buffer's content to a specific area of the display
  esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

/**
 * Callback from lvgl to get touchscreen location events
*/
static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  
  if (go_touch_int && touch.read()) {
    TP_Point t = touch.getPoint(0);
    data->point.x = t.x;
    data->point.y = t.y;
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
  go_touch_int = false;
}

void setup_screen()
{
  //Power on the LCD
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  //Configure the LCD
  pinMode(PIN_LCD_RD, OUTPUT);
  digitalWrite(PIN_LCD_RD, HIGH);
  esp_lcd_i80_bus_handle_t i80_bus = NULL;
  esp_lcd_i80_bus_config_t bus_config = {
      .dc_gpio_num = PIN_LCD_DC,
      .wr_gpio_num = PIN_LCD_WR,
      .clk_src = LCD_CLK_SRC_PLL160M,
      .data_gpio_nums =
          {
              PIN_LCD_D0,
              PIN_LCD_D1,
              PIN_LCD_D2,
              PIN_LCD_D3,
              PIN_LCD_D4,
              PIN_LCD_D5,
              PIN_LCD_D6,
              PIN_LCD_D7,
          },
      .bus_width = 8,
      .max_transfer_bytes = LVGL_LCD_BUF_SIZE * sizeof(uint16_t),
  };
  esp_lcd_new_i80_bus(&bus_config, &i80_bus);

  esp_lcd_panel_io_i80_config_t io_config = {
      .cs_gpio_num = PIN_LCD_CS,
      .pclk_hz = LCD_PIXEL_CLOCK_HZ,
      .trans_queue_depth = 20,
      .on_color_trans_done = notify_lvgl_flush_ready,
      .user_ctx = &disp_drv,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
      .dc_levels =
          {
              .dc_idle_level = 0,
              .dc_cmd_level = 0,
              .dc_dummy_level = 0,
              .dc_data_level = 1,
          },
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));  
  esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_LCD_RES,
      .color_space = ESP_LCD_COLOR_SPACE_RGB,
      .bits_per_pixel = 16,
  };
  esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);
  esp_lcd_panel_reset(panel_handle);
  esp_lcd_panel_init(panel_handle);
  esp_lcd_panel_invert_color(panel_handle, true);

  esp_lcd_panel_swap_xy(panel_handle, true);
  esp_lcd_panel_mirror(panel_handle, false, true);
  // the gap is LCD panel specific, even panels with the same driver IC, can
  // have different gap value
  esp_lcd_panel_set_gap(panel_handle, 0, 35);

  /* LCD backlight PWM */
  ledcSetup(0, 10000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255);

  //Initializes lvgl

  lv_init();
  lv_disp_buf = (lv_color_t *)heap_caps_malloc(LVGL_LCD_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_disp_draw_buf_init(&disp_buf, lv_disp_buf, NULL, LVGL_LCD_BUF_SIZE);
  /*Initialize the display*/
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = LCD_H_RES;
  disp_drv.ver_res = LCD_V_RES;
  disp_drv.flush_cb = lvgl_flush_cb;
  disp_drv.draw_buf = &disp_buf;
  disp_drv.user_data = panel_handle;
  lv_disp_drv_register(&disp_drv);

  /* Register touch brush with LVGL */
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL, 800000);
  inited_touch = touch.init();
  if (inited_touch) {
    touch.setRotation(1);
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_touchpad_read;
    lv_indev_drv_register(&indev_drv);
  }
  attachInterrupt(PIN_TOUCH_INT, [] { go_touch_int = true; lastTouchEvent = millis(); }, FALLING);
  is_initialized_lvgl = true;

  //Initializes the UI
  ui_init();
}

void enable_lcd(void)
{
  esp_lcd_panel_disp_off(panel_handle, false);
  ledcWrite(0, 255);
}

void disable_lcd(void)
{
  esp_lcd_panel_disp_off(panel_handle, true);
  ledcWrite(0, 0);
}

void loop_screen()
{
    unsigned long now = millis();
    static bool prevReady = false;
    if(systemReady){
      if(!prevReady){
        ui_show_home();
      }
      //Screen blanking
      if((now-lastTouchEvent)<SCREEN_BLANK){
          //Screen active
          if(screenBlanked){
              enable_lcd();
              ui_unblank_screen();
              screenBlanked = false;
          }
          }else{
          //Screen not active
          if(!screenBlanked){
              ui_blank_screen();
              screenBlanked = true;
          }
      }
    }
    //lvgl timer call
    if(now >= next_call){
      next_call = now + lv_timer_handler();
    }
    prevReady = systemReady;
}

void set_system_ready()
{
  systemReady = true;
}