#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

#ifdef ARDUINO_ARCH_ESP32

namespace esphome {
namespace govee_h5075 {

static const char *TAG = "govee_h5075";

struct GoveeParseResult {
    float temperature;
    float humidity;
    float battery_level;
};

class GoveeH5075: public Component, public esp32_ble_tracker::ESPBTDeviceListener {
  public:
    void dump_config() override;
    float get_setup_priority() const override { return setup_priority::DATA; }
    void set_temperature(sensor::Sensor *temperature) { temperature_ = temperature; }
    void set_humidity(sensor::Sensor *humidity) { humidity_ = humidity; }
    void set_battery_level(sensor::Sensor *battery_level) { battery_level_ = battery_level; }
    void set_address(uint64_t address) { address_ = address; }
    bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;

  protected:
    uint64_t address_;
    sensor::Sensor *temperature_{nullptr};
    sensor::Sensor *humidity_{nullptr};
    sensor::Sensor *battery_level_{nullptr};

    bool parse_govee(GoveeParseResult &result, const esp32_ble_tracker::ServiceData manufacturer_data);
    optional <GoveeParseResult> parse_service_data(const esp32_ble_tracker::ESPBTDevice &device);
};

}  // namespace govee_h5075
}  // namespace esphome

#endif
