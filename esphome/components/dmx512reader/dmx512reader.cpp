#include "dmx512reader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dmx512reader {

static const char *TAG = "dmx512reader";

void DMX512Reader::update() {
  bool update = false;
  // An update needs to be triggered periodically, if the
  // periodic update option is set
  if ( (((millis() - this->last_update_) > this->update_interval_) && this->periodic_update_)) {
    // Force the refresh rate to be within the spec
    if ((millis() - this->last_update_) > DMX_MIN_INTERVAL_MS) {
      update = true;
    }
  }
  if (update) {
    if (!this->available()) {
      return;
    }
    if (!(this->read_array(this->device_values_, this->max_chan_ + 1))) {
      
      ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
    }
    this->last_update_ = millis();
  }
}

void DMX512Reader::dump_config() { ESP_LOGCONFIG(TAG, "Setting up DMX512 Reader..."); }

void DMX512Reader::setup() {
  for (int i = 0; i < DMX_MSG_SIZE; i++)
    this->device_values_[i] = 0;
  if (this->pin_enable_) {
    ESP_LOGD(TAG, "Enabling RS485 module");
    this->pin_enable_->setup();
    this->pin_enable_->digital_read();
  }
}

void DMX512Sensor::setup() {}

void DMX512Sensor::dump_config() {}

void DMX512Sensor::update() { 
  uint8_t sensorvalue = this->universe_->device_values_[this->channel_];
  //ESP_LOGD(TAG, "Channel '%fV': value '%fV'", this->channel_, sensorvalue);
  this->dmxsensor_->publish_state((float)sensorvalue);

}

float DMX512Sensor::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }
}  // namespace dmx512
}  // namespace esphome
