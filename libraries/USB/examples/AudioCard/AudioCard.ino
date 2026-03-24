// Copyright 2015-2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ESP_I2S.h"
#include "USB.h"
#include "USBAudioCard.h"

#if CONFIG_IDF_TARGET_ESP32S3
#define I2S_BCLK 4
#define I2S_LRCK 5
#define I2S_DOUT 6
#define I2S_DIN  7
#define I2S_WIDTH I2S_DATA_BIT_WIDTH_16BIT
#define UAC_BPS UAC_BPS_16
#else
#define I2S_BCLK 0
#define I2S_LRCK 1
#define I2S_DOUT 2
#define I2S_DIN  3
#define I2S_WIDTH I2S_DATA_BIT_WIDTH_32BIT
#define UAC_BPS UAC_BPS_24
#endif

USBAudioCard uac(48000, UAC_BPS, UAC_SPK_STEREO, UAC_MIC_STEREO);
I2SClass i2s;

// Buffer to hold input (microphone) data
uint8_t inputBuffer[1024];

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
      case ARDUINO_USB_STARTED_EVENT: Serial.println("USB PLUGGED"); break;
      case ARDUINO_USB_STOPPED_EVENT: Serial.println("USB UNPLUGGED"); break;
      case ARDUINO_USB_SUSPEND_EVENT: Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\r\n", data->suspend.remote_wakeup_en); break;
      case ARDUINO_USB_RESUME_EVENT:  Serial.println("USB RESUMED"); break;
      default: break;
    }
  } else if (event_base == ARDUINO_USB_AUDIO_CARD_EVENTS) {
    arduino_usb_audio_card_event_data_t *data = (arduino_usb_audio_card_event_data_t *)event_data;
    switch (event_id) {
      case ARDUINO_USB_AUDIO_CARD_VOLUME_EVENT: Serial.printf("AUDIO VOLUME CH: %d, DB: %d\r\n", data->volume.channel, data->volume.db); break;
      case ARDUINO_USB_AUDIO_CARD_MUTE_EVENT: Serial.printf("AUDIO MUTE CH: %d, MUTED: %d\r\n", data->mute.channel, data->mute.muted); break;
      case ARDUINO_USB_AUDIO_CARD_SAMPLE_RATE_EVENT: Serial.printf("AUDIO SAMPLE RATE: %lu\r\n", data->sample_rate.rate); break;
      case ARDUINO_USB_AUDIO_CARD_INTERFACE_ENABLE_EVENT: Serial.printf("AUDIO INTERFACE: %s, ENABLED: %d\r\n", data->interface_enable.interface?"MIC":"SPK", data->interface_enable.enable); break;
      default: break;
    }
  }
}

void checkButton() {
  // Poll every 50ms
  const uint32_t interval_ms = 50;
  static uint32_t start_ms = 0;
  if (millis() - start_ms < interval_ms) {
    return;
  }
  start_ms += interval_ms;

  static uint32_t btn_prev = 0;
  uint32_t btn = digitalRead(BOOT_PIN);
  if (!btn_prev && btn) {
    // Mute/Unmute UAC2
    uac.mute(UAC_CHAN_MASTER, !uac.mute(UAC_CHAN_MASTER));
  }
  btn_prev = btn;
}

void onSpkData(void *data, uint16_t len) {
  uac.applyVolume(data, len);
  i2s.write((const uint8_t *)data, len);
}

void setup() {
  pinMode(BOOT_PIN, INPUT);
  Serial.begin(115200);

  i2s.setPins(I2S_BCLK, I2S_LRCK, I2S_DOUT, I2S_DIN);
  i2s.begin(I2S_MODE_STD, 48000, I2S_WIDTH, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH);

  uac.onEvent(usbEventCallback);
  uac.onData(onSpkData);
  uac.begin();

  USB.onEvent(usbEventCallback);
  USB.begin();
}

void loop() {
  checkButton();
  // Read 1ms worth of Microphone Data
  size_t toRead = (uac.sampleRate() * uac.micChannels() * uac.bytesPerSample()) / 1000;
  size_t received = i2s.readBytes((char *)inputBuffer, toRead);
  if (received > 0) {
    uac.write(inputBuffer, (uint16_t)received);
  }
}
