#include "driver/i2s_std.h"
#include "usb_device_uac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_timer.h"

static i2s_chan_handle_t tx_handle = NULL;
static i2s_chan_handle_t rx_handle = NULL;

static esp_err_t uac_output_cb(uint8_t *buf, size_t len, void *ctx)
{
    size_t written = 0;
    i2s_channel_write(tx_handle, buf, len, &written, pdMS_TO_TICKS(8));

    return ESP_OK;
}

static esp_err_t uac_input_cb(uint8_t *buf, size_t len, size_t *bytes_read, void *ctx)
{
    esp_err_t ret = i2s_channel_read(rx_handle, buf, len, bytes_read, pdMS_TO_TICKS(20));
    if (ret == ESP_ERR_TIMEOUT)
    {
        *bytes_read = 0;
        memset(buf, 0, len);
    }
    return ret;
}

void app_main(void)
{
    i2s_chan_config_t tx_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_SLAVE);
    tx_cfg.auto_clear_after_cb = true;
    i2s_new_channel(&tx_cfg, &tx_handle, NULL);

    i2s_chan_config_t rx_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_SLAVE);
    i2s_new_channel(&rx_cfg, NULL, &rx_handle);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000), 
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .bclk = GPIO_NUM_8,
            .ws = GPIO_NUM_7,
            .dout = GPIO_NUM_43,
            .din = GPIO_NUM_44,
            .mclk = I2S_GPIO_UNUSED,
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_init_std_mode(rx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
    i2s_channel_enable(rx_handle);

    uac_device_config_t uac_cfg = {
        .output_cb = uac_output_cb,
        .input_cb = uac_input_cb,
    };
    uac_device_init(&uac_cfg);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}