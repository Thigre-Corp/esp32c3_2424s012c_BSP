/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: esp32c3_2424s012c
 */

#pragma once

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "bsp/config.h"
#include "driver/i2c_master.h"
#include "esp_vfs_fat.h"
#include "iot_button.h"
#include "bsp/display.h"


#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
#include "lvgl.h"
#include "esp_lvgl_port.h"
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0
/**************************************************************************************************
 *  BSP Board Name
 **************************************************************************************************/

/** @defgroup boardname Board Name
 *  @brief BSP Board Name
 *  @{
 */
#define BSP_BOARD_ESP32C3_2424S012C
/** @} */ // end of boardname

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/

/** @defgroup capabilities Capabilities
 *  @brief BSP Capabilities
 *  @{
 */
#define BSP_CAPS_DISPLAY          1
#define BSP_CAPS_TOUCH            1
#define BSP_CAPS_BUTTONS          1
#define BSP_CAPS_KNOB             0
#define BSP_CAPS_AUDIO            0
#define BSP_CAPS_AUDIO_SPEAKER    0
#define BSP_CAPS_AUDIO_MIC        0
#define BSP_CAPS_SDCARD           0
#define BSP_CAPS_LED              0
#define BSP_CAPS_CAMERA           0
#define BSP_CAPS_BAT              0
#define BSP_CAPS_IMU              0
#define BSP_CAPS_HUMITURE         0
/** @} */ // end of capabilities

/**************************************************************************************************
 *  Board pinout
 **************************************************************************************************/

/** @defgroup g01_i2c I2C
 *  @brief I2C BSP API
 *  @{
 */
#define BSP_I2C_SCL     (GPIO_NUM_5)
#define BSP_I2C_SDA     (GPIO_NUM_4)
/** @} */ // end of i2c

/** @defgroup g04_display Display and Touch
 *  @brief Display BSP API
 *  @{
 */
#define BSP_LCD_PCLK         (GPIO_NUM_6)
#define BSP_LCD_DATA0        (GPIO_NUM_7)
#define BSP_LCD_DC           (GPIO_NUM_2)
#define BSP_LCD_CS           (GPIO_NUM_10)
#define BSP_LCD_RST          (GPIO_NUM_1)
#define BSP_LCD_BACKLIGHT    (GPIO_NUM_3)
#define BSP_LCD_TOUCH_INT    (GPIO_NUM_0)
/** @} */ // end of display

/** @defgroup g05_buttons Buttons
 *  @brief Buttons BSP API
 *  @{
 */
#define BSP_BUTTON_BTN_IO    (GPIO_NUM_8)
/** @} */ // end of buttons

/** \addtogroup g05_buttons
 *  @{
 */
/* Button definitions */
typedef enum {
    BSP_BUTTON_BTN,
    BSP_BUTTON_NUM
} bsp_button_t;
/** @} */ // end of buttons

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup g01_i2c
 *  @{
 */

/**************************************************************************************************
 *
 * I2C interface
 *
 **************************************************************************************************/
#define BSP_I2C_NUM     CONFIG_BSP_I2C_NUM

/**
 * @brief Init I2C driver
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *      - ESP_FAIL              I2C driver installation error
 *
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief Deinit I2C driver and free its resources
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *
 */
esp_err_t bsp_i2c_deinit(void);

/**
 * @brief Get I2C driver handle
 *
 * @return
 *      - I2C handle
 */
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

/** @} */ // end of i2c

/** @defgroup g02_storage SD Card and SPIFFS
 *  @brief SPIFFS and SD card BSP API
 *  @{
 */

/** @} */ // end of storage

/** \addtogroup g04_display
 *  @{
 */

/**************************************************************************************************
 *
 * LCD interface
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling bsp_display_lock() before calling any LVGL API (lv_...) and then give the mutex with
 * bsp_display_unlock().
 *
 * Display's backlight must be enabled explicitly by calling bsp_display_backlight_on()
 **************************************************************************************************/
#define BSP_LCD_PIXEL_CLOCK_HZ     (40 * 1000 * 1000)
#define BSP_LCD_SPI_NUM            (SPI2_HOST)

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)

/**
 * @brief BSP display (LVGL) configuration structure
 */
typedef struct {
    lvgl_port_cfg_t lvgl_port_cfg;  /*!< LVGL port configuration */
    uint32_t        buffer_size;    /*!< Size of the buffer for the screen in pixels */
    bool            double_buffer;  /*!< True, if should be allocated two buffers */
    struct {
        unsigned int buff_dma: 1;    /*!< Allocated LVGL buffer will be DMA capable */
        unsigned int buff_spiram: 1; /*!< Allocated LVGL buffer will be in PSRAM */
        unsigned int sw_rotate: 1;   /*!< Use software rotation (slower), The feature is unavailable under avoid-tear mode */
    } flags;
} bsp_display_cfg_t;

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start(void);

/**
 * @brief Initialize display
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @param cfg display configuration
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg);

/**
 * @brief Get pointer to input device (touch, buttons, ...)
 *
 * @note The LVGL input device is initialized in bsp_display_start() function.
 *
 * @return Pointer to LVGL input device or NULL when not initialized
 */
lv_indev_t *bsp_display_get_input_dev(void);

/**
 * @brief Take LVGL mutex
 *
 * @param timeout_ms Timeout in [ms]. 0 will block indefinitely.
 * @return true  Mutex was taken
 * @return false Mutex was NOT taken
 */
bool bsp_display_lock(uint32_t timeout_ms);

/**
 * @brief Give LVGL mutex
 *
 */
void bsp_display_unlock(void);

/**
 * @brief Set display enter sleep mode
 *
 * All the display (LCD, backlight, touch) will enter sleep mode.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t bsp_display_enter_sleep(void);

/**
 * @brief Set display exit sleep mode
 *
 * All the display (LCD, backlight, touch) will exit sleep mode.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if this function is not supported by the panel
 */
esp_err_t bsp_display_exit_sleep(void);

/**
 * @brief Rotate screen
 *
 * Display must be already initialized by calling bsp_display_start()
 *
 * @param[in] disp Pointer to LVGL display
 * @param[in] rotation Angle of the display rotation
 */
void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation);
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

/** @} */ // end of display

/** \addtogroup g05_buttons
 *  @{
 */

/**************************************************************************************************
 *
 * Button
 *
 **************************************************************************************************/

/**
 * @brief Initialize all buttons
 *
 * Returned button handlers must be used with espressif/button component API
 *
 * @param[out] btn_array      Output button array
 * @param[out] btn_cnt        Number of button handlers saved to btn_array, can be NULL
 * @param[in]  btn_array_size Size of output button array. Must be at least BSP_BUTTON_NUM
 * @return
 *     - ESP_OK               All buttons initialized
 *     - ESP_ERR_INVALID_ARG  btn_array is too small or NULL
 *     - ESP_FAIL             Underlaying iot_button_create failed
 */
esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size);

/** @} */ // end of buttons

#ifdef __cplusplus
}
#endif