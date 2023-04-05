#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

#define GP8302_CONFIG_CURRENT_REG 0x02
#define OUTPUT_RANGE 0X01

namespace esphome {
namespace GP8403 {
/**
 * @enum eOutPutRange_t
 * @brief Analog voltage output range select
 */
enum class eOutPutRange_t {
  eOutputRange5V = 0X00,
  eOutputRange10V = 0X11,
};

enum class ChannelSelect : uint8_t { Channel1 = 0, Channel2 = 1, Both = 2 };

class GP8403 : public i2c::I2CDevice, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  /**
   * @fn setDACOutVoltage
   * @brief Set output DAC voltage of different channels
   * @param data The voltage value to be output
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   * @return NONE
   */
  void setDACOutVoltage(float state);
  void setDACOutRange();
  void set_channel(int channel_) { channel = static_cast<ChannelSelect>(channel_); }
  void set_OutVoltage(int max_power);

 protected:
  uint16_t voltage = 0;
  ChannelSelect channel{ChannelSelect::Both};

  void startSignal(void);
  void stopSignal(void);
};

class GP8403Output;

class GP8403Output : public output::FloatOutput {
 public:
  void set_parent(GP8403 *parent) { parent_ = parent; }

 protected:
  GP8403 *parent_;
  void write_state(float state) override;
};
}  // namespace GP8403
}  // namespace esphome
