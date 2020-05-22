#include "govee_h5075.h"
#include "esphome/core/log.h"

#ifdef ARDUINO_ARCH_ESP32

namespace esphome {
namespace govee_h5075 {

void GoveeH5075::dump_config() {
	ESP_LOGCONFIG(TAG, "Govee H5075");
	LOG_SENSOR("  ", "Temperature", this->temperature_);
	LOG_SENSOR("  ", "Humidity", this->humidity_);
	LOG_SENSOR("  ", "Battery Level", this->battery_level_);
}

bool GoveeH5075::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
    if (device.address_uint64() != this->address_)
        return false;

    auto res = parse_service_data(device);

    if (!res.has_value())
        return false;

    this->temperature_->publish_state(res->temperature);
    this->humidity_->publish_state(res->humidity);
    this->battery_level_->publish_state(res->battery_level);
    return true;
}

bool GoveeH5075::parse_govee(GoveeParseResult &result, const esp32_ble_tracker::ServiceData manufacturer_data) {
	if (!manufacturer_data.uuid.contains(0x88, 0xEC))
		return false;

	// Combine bytes 1, 2, and 3 into a 32 bit integer, which will hold the temperature and humitidy data
    uint32_t temp_humidity_data = (manufacturer_data.data[1] << 16 |
    	manufacturer_data.data[2] << 8 | manufacturer_data.data[3]);

    // Extract the temperature data (first 3 digits)
    result.temperature = (float) temp_humidity_data / 10000.0;

    // Extract the humidity data (last 3 digits)
    result.humidity = (float) (temp_humidity_data % 1000) / 10.0;

    result.battery_level = manufacturer_data.data[4];

    return true;
}

optional <GoveeParseResult> GoveeH5075::parse_service_data(const esp32_ble_tracker::ESPBTDevice &device) {
    GoveeParseResult result;
    bool success = false;

    for (const esp32_ble_tracker::ServiceData &manufacturer_data: device.get_manufacturer_datas()) {
        if (manufacturer_data.data.size() == 6) {
            if (parse_govee(result, manufacturer_data))
            	success = true;
        }
    }

    if (success)
	    return result;
	
	return {};
}



}  // namespace govee_h5075
}  // namespace esphome

#endif