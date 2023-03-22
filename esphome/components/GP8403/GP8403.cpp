#include "esphome/core/log.h"
#include "GP8403.h"

#define GP8302_STORE_TIMING_HEAD 0x02  ///< Store function timing start head
#define GP8302_STORE_TIMING_ADDR 0x10  ///< The first address for entering store timing
#define GP8302_STORE_TIMING_CMD1 0x03  ///< The command 1 to enter store timing
#define GP8302_STORE_TIMING_CMD2 0x00  ///< The command 2 to enter store timing
#define GP8302_STORE_TIMING_DELAY 10   ///< Store procedure interval delay time: 10ms, more than 7ms
#define GP8302_STORE_TIMING_DELAY 10   ///< Store procedure interval delay time: 10ms, more than 7ms
#define I2C_CYCLE_TOTAL 5              ///< Total I2C communication cycle
#define I2C_CYCLE_BEFORE 1             ///< The first half cycle 2 of the total I2C communication cycle
#define I2C_CYCLE_AFTER 2              ///< The second half cycle 3 of the total I2C communication cycle

namespace esphome {
namespace GP8403 {

static const char *TAG = "GP8403";

void GP8403::dump_config() { ESP_LOGCONFIG(TAG, "GP8403 0-10V output", this); }

void GP8403::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GP8403 0-10V output", this->address_);
  setDACOutRange();
}
/**
 * @fn setDACOutVoltage
 * @brief Set output DAC voltage of different channels
 * @param data The voltage value to be output
 * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
 * @return NONE
 */
void GP8403::setDACOutVoltage(float state) {
  std::vector<uint8_t> data;
  uint16_t dataTransmission;
  dataTransmission = ((uint16_t) (state * 4095)) << 4;

  if (this->channel == ChannelSelect::Channel1) {
    data.push_back(GP8302_CONFIG_CURRENT_REG);
    data.push_back(dataTransmission & 0xFF);
    data.push_back((dataTransmission >> 8) & 0xFF);
  } else if (this->channel == ChannelSelect::Channel2) {
    data.push_back(GP8302_CONFIG_CURRENT_REG << 1);
    data.push_back(dataTransmission & 0xFF);
    data.push_back((dataTransmission >> 8) & 0xFF);
  } else {
    data.push_back(GP8302_CONFIG_CURRENT_REG);
    data.push_back(dataTransmission & 0xFF);
    data.push_back((dataTransmission >> 8) & 0xFF);
    data.push_back(dataTransmission & 0xFF);
    data.push_back((dataTransmission >> 8) & 0xFF);
  }
  if (this->write(data.data(), data.size()) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Error writing to GP8403");
  }
}

/**
 * @fn set_max_power
 * @brief Set DAC output range
 * @param range DAC output range
 * @return NONE
 */
void GP8403::setDACOutRange() {
  uint8_t range;
  std::vector<uint8_t> data;
  if (voltage == 5000) {
    range = (uint8_t) eOutPutRange_t::eOutputRange5V;
  } else {
    range = (uint8_t) eOutPutRange_t::eOutputRange10V;
  }
  data.push_back(OUTPUT_RANGE);
  data.push_back(range);
  if (this->write(data.data(), data.size()) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Error writing to GP8403");
  }
  // this->write_register(OUTPUT_RANGE, &range, 1, true);
}

void GP8403::set_OutVoltage(int max_power) {
  if (max_power <= 5) {
    voltage = 5000;
  } else {
    voltage = 10000;
  }
}

void GP8403::startSignal(void) {
  //digitalWrite(_scl, HIGH);
  //digitalWrite(_sda, HIGH);
  //delayMicroseconds(I2C_CYCLE_BEFORE);
  //digitalWrite(_sda, LOW);
  //delayMicroseconds(I2C_CYCLE_AFTER);
  //digitalWrite(_scl, LOW);
  //delayMicroseconds(I2C_CYCLE_TOTAL);
}

void GP8403::stopSignal(void) {
  //digitalWrite(_sda, LOW);
  //delayMicroseconds(I2C_CYCLE_BEFORE);
  //digitalWrite(_scl, HIGH);
  //delayMicroseconds(I2C_CYCLE_TOTAL);
  //digitalWrite(_sda, HIGH);
  //delayMicroseconds(I2C_CYCLE_TOTAL);
}

void GP8403Output::write_state(float state) { this->parent_->setDACOutVoltage(state); }

}  // namespace GP8403
}  // namespace esphome
