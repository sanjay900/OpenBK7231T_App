
#include "new_http.h"
#include "../cJSON/cJSON.h"
#include "../new_pins.h"
#include "../mqtt/new_mqtt.h"
#include "../cmnds/cmd_public.h"

typedef enum {
	/// @brief Switch
	RELAY = 0,

	/// @brief 
	LIGHT_ON_OFF,

	/// @brief Single PWM
	LIGHT_PWM,

	/// @brief 2 PWM setup (brightness and temperature)
	LIGHT_PWMCW,

	/// @brief RGB (3 PWM)
	LIGHT_RGB,

	/// @brief RGB + temperature (5 PWM or LED driver)
	LIGHT_RGBCW,

	/// @brief Power sensors (voltage, current, power)
	POWER_SENSOR,

	/// @Brief Binary Sensor
	BINARY_SENSOR,

	/// @brief Temperature sensor
	TEMPERATURE_SENSOR,
	/// @brief Humidity sensor
	HUMIDITY_SENSOR,

	/// @brief Battery level sensor in perc
	BATTERY_SENSOR,
	/// @brief Battery votage sensor in mV
	BATTERY_VOLTAGE_SENSOR,

	/// @brief CO2 sensor in ppm
	CO2_SENSOR,
	/// @brief TVOC sensor in ppb
	TVOC_SENSOR

} ENTITY_TYPE;

//unique_id is defined in hass_populate_unique_id and is based on CFG_GetDeviceName() whose size is CGF_DEVICE_NAME_SIZE.
//Sample unique_id would be deviceName_entityType_index.
//Currently supported entityType is `relay` or `light` - 5 char.
#define HASS_UNIQUE_ID_SIZE     (CGF_DEVICE_NAME_SIZE + 1 + 5 + 1 + 4)

//channel is based on unique_id (see hass_populate_device_config_channel)
#define HASS_CHANNEL_SIZE       (HASS_UNIQUE_ID_SIZE + 32)

//Size of JSON (1 less than MQTT queue holding)
#define HASS_JSON_SIZE          (MQTT_PUBLISH_ITEM_VALUE_LENGTH - 1)

/// @brief HomeAssistant device discovery information
typedef struct HassDeviceInfo_s {
	char unique_id[HASS_UNIQUE_ID_SIZE];
	char channel[HASS_CHANNEL_SIZE];
	char json[HASS_JSON_SIZE];

	cJSON* root;
	cJSON* device;
	cJSON* ids;
} HassDeviceInfo;

void hass_print_unique_id(http_request_t* request, const char* fmt, ENTITY_TYPE type, int index);
HassDeviceInfo* hass_init_relay_device_info(int index, ENTITY_TYPE type);
HassDeviceInfo* hass_init_light_device_info(ENTITY_TYPE type);
HassDeviceInfo* hass_init_power_sensor_device_info(int index);
HassDeviceInfo* hass_init_binary_sensor_device_info(int index);
HassDeviceInfo* hass_init_sensor_device_info(ENTITY_TYPE type, int channel);
const char* hass_build_discovery_json(HassDeviceInfo* info);
void hass_free_device_info(HassDeviceInfo* info);
