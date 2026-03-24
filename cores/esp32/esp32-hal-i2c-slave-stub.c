// Minimal ESP32-C6/IDF6 stub: Arduino I2C slave mode is not used by this project
// and the legacy implementation is not compatible with ESP-IDF 6.

#include "soc/soc_caps.h"

#if SOC_I2C_SUPPORT_SLAVE

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp32-hal-log.h"
#include "esp32-hal-i2c-slave.h"

static esp_err_t i2c_slave_not_supported(void) {
  log_e("Arduino I2C slave mode is not supported on this ESP32-C6 / ESP-IDF 6 build");
  return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t i2cSlaveAttachCallbacks(uint8_t num, i2c_slave_request_cb_t request_callback, i2c_slave_receive_cb_t receive_callback, void *arg) {
  (void)num;
  (void)request_callback;
  (void)receive_callback;
  (void)arg;
  return i2c_slave_not_supported();
}

esp_err_t i2cSlaveInit(uint8_t num, int sda, int scl, uint16_t slaveID, uint32_t frequency, size_t rx_len, size_t tx_len) {
  (void)num;
  (void)sda;
  (void)scl;
  (void)slaveID;
  (void)frequency;
  (void)rx_len;
  (void)tx_len;
  return i2c_slave_not_supported();
}

esp_err_t i2cSlaveDeinit(uint8_t num) {
  (void)num;
  return ESP_OK;
}

size_t i2cSlaveWrite(uint8_t num, const uint8_t *buf, uint32_t len, uint32_t timeout_ms) {
  (void)num;
  (void)buf;
  (void)len;
  (void)timeout_ms;
  i2c_slave_not_supported();
  return 0;
}

#endif
