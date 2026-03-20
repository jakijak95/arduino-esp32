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

#pragma once

#include "soc/soc_caps.h"
#if SOC_USB_OTG_SUPPORTED
#include "sdkconfig.h"
#if CONFIG_TINYUSB_AUDIO_ENABLED
#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(ARDUINO_USB_AUDIO_CARD_EVENTS);

typedef enum {
  ARDUINO_USB_AUDIO_CARD_ANY_EVENT = ESP_EVENT_ANY_ID,
  ARDUINO_USB_AUDIO_CARD_VOLUME_EVENT,
  ARDUINO_USB_AUDIO_CARD_MUTE_EVENT,
  ARDUINO_USB_AUDIO_CARD_SAMPLE_RATE_EVENT,
  ARDUINO_USB_AUDIO_CARD_INTERFACE_ENABLE_EVENT,
  ARDUINO_USB_AUDIO_CARD_MAX_EVENT,
} arduino_usb_audio_card_event_t;

typedef enum {
  UAC_CHAN_MASTER,
  UAC_CHAN_LEFT,
  UAC_CHAN_RIGHT
} UAC_Channel;

typedef enum {
  UAC_INTERFACE_SPK,
  UAC_INTERFACE_MIC
} UAC_Interface;

typedef union {
  struct {
    UAC_Channel channel;
    int8_t db;
  } volume;
  struct {
    UAC_Channel channel;
    bool muted;
  } mute;
  struct {
    uint32_t rate;
  } sample_rate;
  struct {
    UAC_Interface interface;
    bool enable;
  } interface_enable;
} arduino_usb_audio_card_event_data_t;

typedef enum {
  UAC_SPK_NONE,
  UAK_SPK_MONO,
  UAC_SPK_STEREO
} UAC_SPK_Channels;

typedef enum {
  UAC_MIC_NONE,
  UAK_MIC_MONO,
  UAC_MIC_STEREO
} UAC_MIC_Channels;

typedef enum {
  UAC_BPS_16 = 16,
  UAC_BPS_24 = 24,
  UAC_BPS_32 = 32
} UAC_Bits_Per_Sample;

typedef void (*arduino_usb_audio_card_data_handler_t)(void *data, uint16_t len);

class USBAudioCard {
public:
  USBAudioCard(uint32_t sample_rate, UAC_Bits_Per_Sample bps, UAC_SPK_Channels spk_channels=UAC_SPK_STEREO, UAC_MIC_Channels mic_channels=UAC_MIC_STEREO);
  ~USBAudioCard();

  bool begin();
  void end();

  UAC_SPK_Channels speakerChannels();
  UAC_MIC_Channels micChannels();
  UAC_Bits_Per_Sample bitsPerSample();
  uint8_t bytesPerSample();
  uint32_t sampleRate();
  bool mute(UAC_Channel channel);
  bool mute(UAC_Channel channel, bool muted); // UAC2 Only
  int8_t volume(UAC_Channel channel);
  bool volume(UAC_Channel channel, int8_t volume_db); // UAC2 Only

  // Apply currently set channel volumes directly to audio data (if DAC does not support setting volume)
  void applyVolume(void *data, uint16_t len);

  void onData(arduino_usb_audio_card_data_handler_t callback);
  uint16_t write(const void *data, uint16_t len);

  void onEvent(esp_event_handler_t callback);
  void onEvent(arduino_usb_audio_card_event_t event, esp_event_handler_t callback);

private:

};


#endif /* CONFIG_TINYUSB_AUDIO_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
