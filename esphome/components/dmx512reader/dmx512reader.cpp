#include "dmx512reader.h"
#include "esphome/core/log.h"

namespace esphome {
	namespace dmx512reader {

		static const char* TAG = "dmx512reader";

		void DMX512Reader::loop() {
			bool update = false;
			update = true;

			uint8_t tmp_device_values[DMX_MSG_SIZE];
			//// An update needs to be triggered periodically, if the
			//// periodic update option is set
			//if ((((millis() - this->last_update_) > this->update_interval_) && this->periodic_update_)) {
			//	//ESP_LOGD(TAG, "Update UART read array");
			//  // Force the refresh rate to be within the spec
			//	if ((millis() - this->last_update_) > DMX_MIN_INTERVAL_MS) {
			//		update = true;
			//	}
			//}
			if (update) {
				
				if (!this->available()) {
					//ESP_LOGD(TAG, "number of bytes available: %d", this->available());
					return;
				}
				//if (!(this->read_array(this->device_values_, this->max_chan_ + 1))) {
				if (!(this->read_array(tmp_device_values, this->max_chan_ + 1))) {

					ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
				}
				else
				{
					ESP_LOGD(TAG, "Device value 20: %u", device_values_[20]);
				}
				// Check if DMX protocol, than the first bytes equals 0, otherwise it's RDM or another protocol
				if (tmp_device_values[0] == 0)
					std::copy(std::begin(tmp_device_values), std::end(tmp_device_values), std::begin(this->device_values_));
					//this->device_values_ = tmp_device_values;
				else
					ESP_LOGD(TAG, "No DMX");
				this->last_update_ = millis();
			}
		}

		void DMX512Reader::dump_config() { ESP_LOGCONFIG(TAG, "Setting up DMX512 Reader..."); }

		void DMX512Reader::setup() {
			for (int i = 0; i < DMX_MSG_SIZE; i++)
				this->device_values_[i] = 0;
		}

		void DMX512Sensor::setup() {}

		void DMX512Sensor::dump_config() {}

		void DMX512Sensor::update() {
			uint8_t sensorvalue = this->universe_->device_values_[this->channel_];
			//ESP_LOGD(TAG, "Channel '%f': value '%f'", this->channel_, sensorvalue);
			this->dmxsensor_->publish_state((float)sensorvalue);

		}

		float DMX512Sensor::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }
	}  // namespace dmx512
}  // namespace esphome
