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

struct LoopOutputBools {
  bool OutputSin;
  bool OutputSquare;
  bool OutputTriangle;
}; 

class OutputVariable {
 public:
  uint16_t amp;
  uint16_t freq;
  uint16_t offset;
  int8_t dutyCycle;
};

enum class ChannelSelect : uint8_t { Channel1 = 0, Channel2 = 1, Both = 2 };

class GP8403 : public i2c::I2CDevice, public Component {
 public:
  LoopOutputBools BooleansChannel1;
  LoopOutputBools BooleansChannel2;
  LoopOutputBools BooleansChannelBoth;

  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  /**
   * @fn setDACOutVoltage
   * @brief Set output DAC voltage of different channels
   * @param data The voltage value to be output
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   * @return NONE
   */
<<<<<<< HEAD
  void setDACOutVoltage(float state, ChannelSelect channel);
  /**
   * @fn setDACOutVoltage
   * @brief Set output DAC voltage of different channels
   * @param data The voltage value to be output
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   * @return NONE
   */
  void setDACOutRange();
  /**
   * @brief Call the function to set the outputvoltage
   * @param max_power set the maximum power
   */
=======
  void setDACOutVoltage(float state);
  void setDACOutRange();
  void set_channel(int channel_) { channel = static_cast<ChannelSelect>(channel_); }
>>>>>>> parent of 525c1df (Update output en init.py so that also channel can be set in the output. Also included a test.yaml)
  void set_OutVoltage(int max_power);
  /**
   * @brief Call the function to output sine wave
   * @param amp Set sine wave amplitude Vp
   * @param freq Set sine wave frequency f
   * @param offset Set sine wave DC offset Voffset
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   */
  void outputSin(uint16_t amp, uint16_t freq, uint16_t offset, ChannelSelect channel);
  /**
   * @brief Call the function to output triangle wave
   * @param amp Set triangle wave amplitude Vp
   * @param freq Set triangle wave frequency f
   * @param offset Set triangle wave DC offset Voffset
   * @param dutyCycle Set triangle (sawtooth) wave duty cycle
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   */
  void outputTriangle(uint16_t amp, uint16_t freq, uint16_t offset, int8_t dutyCycle, ChannelSelect channel);
  /**
   * @brief Call the function to output square wave
   * @param amp Set square wave amplitude Vp
   * @param freq Set square wave frequency f
   * @param offset Set square wave DC offset Voffset
   * @param dutyCycle Set square wave duty cycle
   * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
   */
  void outputSquare(uint16_t amp, uint16_t freq, uint16_t offset, int8_t dutyCycle, ChannelSelect channel);
 protected:
  uint16_t voltage = 0;
<<<<<<< HEAD
  void sendData(uint16_t dataTransmission, ChannelSelect channel);
=======
  ChannelSelect channel{ChannelSelect::Both};

  void startSignal(void);
  void stopSignal(void);
>>>>>>> parent of 525c1df (Update output en init.py so that also channel can be set in the output. Also included a test.yaml)
};

class GP8403Output;

class GP8403Output : public output::FloatOutput {
 public:
  void set_parent(GP8403 *parent) { parent_ = parent; }
<<<<<<< HEAD
  void set_channel(int channel_) { channel = static_cast<ChannelSelect>(channel_); }

  void set_output_sin(bool output_sin);
  void set_output_square(bool output_square);
  void set_output_triangle(bool output_triangle);

  ChannelSelect channel{ChannelSelect::Both};
=======
>>>>>>> parent of 525c1df (Update output en init.py so that also channel can be set in the output. Also included a test.yaml)

 protected:
  GP8403 *parent_;
  void write_state(float state) override;
};
}  // namespace GP8403
}  // namespace esphome
