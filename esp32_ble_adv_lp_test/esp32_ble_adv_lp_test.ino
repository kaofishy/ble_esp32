#include "Arduino.h"
#include "esp32-hal-bt.h"
#include "esp_bt.h"
#include "hci.hpp"

// undocumented ESP32 bt controller function
extern "C" bool btdm_power_state_active(void);

template<typename T>
void send_hci_command(const T &command) {
	uint8_t buffer[256] = {0x01};
	auto size = hci::write_hci_command(&buffer[1], 256, command)+1;
	esp_bt_controller_wakeup_request();
	while(!esp_vhci_host_check_send_available());
	esp_vhci_host_send_packet(buffer, size);
}

void setup()
{
	Serial.begin(921600);
	Serial.println("\n\nESP32 low power BLE advertising test");
}

// empty VHCI callbacks
const esp_vhci_host_callback callback = {
		[](){},
		[](uint8_t*, uint16_t)->int{
			return 0;
		}
};

void loop()
{
	// initialize bluetooth
	btStart();
	esp_vhci_host_register_callback(&callback);
	send_hci_command(hci::reset());
	send_hci_command(hci::le_set_advertising_parameters(1500, 1700, 0x00, 0x00, 0x00, {}, 0x07,
			0x00));
	constexpr std::array<uint8_t, 31> adv_data = {
			0x06, 0x09, 'e', 's', 'p', '3', '2' // complete local name
	};
	send_hci_command(hci::le_set_advertising_data(31, adv_data));
	send_hci_command(hci::le_set_advertise_enable(1));
	// wait until advertising finishes
	// this only works when bt sleep is enabled
	while(btdm_power_state_active());
	btStop();
	esp_sleep_enable_timer_wakeup(1000 * 1000);
	esp_light_sleep_start();
	Serial.println("Waking");
}
