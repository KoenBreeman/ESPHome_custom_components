#include "esphome/core/log.h"
#include "DFR0971.h"

#define GP8302_STORE_TIMING_HEAD            0x02  ///< Store function timing start head
#define GP8302_STORE_TIMING_ADDR            0x10  ///< The first address for entering store timing
#define GP8302_STORE_TIMING_CMD1            0x03  ///< The command 1 to enter store timing
#define GP8302_STORE_TIMING_CMD2            0x00  ///< The command 2 to enter store timing
#define GP8302_STORE_TIMING_DELAY           10    ///< Store procedure interval delay time: 10ms, more than 7ms
#define GP8302_STORE_TIMING_DELAY           10    ///< Store procedure interval delay time: 10ms, more than 7ms
#define I2C_CYCLE_TOTAL                     5     ///< Total I2C communication cycle
#define I2C_CYCLE_BEFORE                    1     ///< The first half cycle 2 of the total I2C communication cycle
#define I2C_CYCLE_AFTER                     2     ///< The second half cycle 3 of the total I2C communication cycle

namespace esphome {
	namespace DFR0971 {
		void DFR0971::dump_config() { ESP_LOGCONFIG(TAG, "DFR0971 0-10V output"); }

		void DFR0971::setup() {
			ESP_LOGCONFIG(TAG, "Setting up DFR0971 0-10V output", this->address_);
		}
    /**
     * @fn setDACOutVoltage
     * @brief Set output DAC voltage of different channels
     * @param data The voltage value to be output
     * @param channel Output channel. 0: channel 0; 1: channel 1; 2: all the channels
     * @return NONE
     */
    void DFR0971::setDACOutVoltage(uint16_t data, uint8_t channel)
		{
      if (channel == 0) {
        write_bytes(GP8302_CONFIG_CURRENT_REG, data & 0xff, 0xff);
        write_bytes(GP8302_CONFIG_CURRENT_REG, (data >> 8) & 0xff, 0xff);
      } else if (channel == 1) {
        write_bytes(GP8302_CONFIG_CURRENT_REG << 1, data & 0xff, 0xff);
        write_bytes(GP8302_CONFIG_CURRENT_REG << 1, (data >> 8) & 0xff, 0xff);
       
      } else {
        write_bytes(GP8302_CONFIG_CURRENT_REG, data & 0xff, 0xff);
        write_bytes(GP8302_CONFIG_CURRENT_REG, (data >> 8) & 0xff, 0xff);
        write_bytes(GP8302_CONFIG_CURRENT_REG << 1, data & 0xff, 0xff);
        write_bytes(GP8302_CONFIG_CURRENT_REG << 1, (data >> 8) & 0xff, 0xff);
      }
		}

		void DFR0971Output::dump_config() {
			LOG_SWITCH("", "DFR0971 Output", this);

		}

    /**
     * @fn set_max_power
     * @brief Set DAC output range
     * @param range DAC output range
     * @return NONE
     */
		void DFR0971Output::set_max_power(float state)
		{
			if (state <= 5)
			{
				voltage = 5000;
			}
			else {
				voltage = 10000;
			}
		}

		void DFR0971Output::set_level(float state)
		{
			uint16_t dataTransmission;
			if ((float)state * 1000 > voltage)
				dataTransmission = voltage;
			else
				dataTransmission = (uint16_t)(((float)state * 1000 / voltage) * 4095);

			dataTransmission = dataTransmission << 4;

			if (!this->parent_->setDACOutVoltage(dataTransmission, this->channel)) {
				publish_state(false);
			}
			else {
				publish_state(state);
			}
		}
	}
}  // namespace DFR0971
}  // namespace esphome
