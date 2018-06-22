#include "Arduino.h"

#include <array>
#include <cstdint>
#include <cstdlib>

#include <thread>

#include <nvs_flash.h>
#include <esp_bt.h>
#include <hcidefs.h>
#include <hcimsgs.h>


#include "libbluetoe.h"

// quick and dirty circular tasks fifo
#if 0
class {
	using FuncType = void(*)(uint8_t*);
	enum{
		SIZE=10
	};
	std::array<FuncType, SIZE> tasks ;
	std::array<uint8_t[512], SIZE> data;
	size_t begin;
	size_t end;
public:
	void push(const FuncType f) {
		tasks[end] = f;
		end = (end==SIZE-1) ? 0 : end+1;
	}
	auto pop() -> FuncType {
		if(begin==end) return nullptr;
		auto f = tasks[begin];
		begin = (begin==SIZE-1) ? 0 : begin+1;
		return f;
	}
	size_t size() {
		return end - begin;
	}
} ble_tasks_queue;
#endif

#if 1
std::uint8_t read_blob_handler( std::size_t offset, std::size_t, std::uint8_t* out_buffer, std::size_t& out_size )
{
    if ( offset == 0 )
    {
        *out_buffer = 42;
        out_size    = 1;

        return bluetoe::error_codes::success;
    }

    return bluetoe::error_codes::invalid_offset;
}

std::uint8_t read_handler( std::size_t, std::uint8_t* out_buffer, std::size_t& out_size )
{
    *out_buffer = 42;
    out_size    = 1;

    return bluetoe::error_codes::success;
}

std::uint8_t write_blob_handler( std::size_t, std::size_t, const std::uint8_t* )
{
    return bluetoe::error_codes::write_not_permitted;
}

std::uint8_t write_handler( std::size_t, const std::uint8_t* )
{
    return bluetoe::error_codes::write_not_permitted;
}

struct static_handler {
    static constexpr std::uint8_t code_word[] = { 'a', 'b', 'c' };

    static std::uint8_t read( std::size_t offset, std::size_t read_size, std::uint8_t* out_buffer, std::size_t& out_size )
    {
        if ( offset > sizeof( code_word ) )
            return bluetoe::error_codes::invalid_offset;

        out_size = std::min( read_size, sizeof( code_word ) - offset );
        std::copy( &code_word[ offset ], &code_word[ offset + out_size], out_buffer );

        return bluetoe::error_codes::success;
    }

    static std::uint8_t write( std::size_t, std::size_t, const std::uint8_t* )
    {
        return bluetoe::error_codes::write_not_permitted;
    }
};

constexpr std::uint8_t static_handler::code_word[];

struct handler {
    std::uint8_t read_blob( std::size_t, std::size_t, std::uint8_t*, std::size_t& )
    {
        return bluetoe::error_codes::success;
    }

    std::uint8_t write_blob( std::size_t, std::size_t, const std::uint8_t* )
    {
        return bluetoe::error_codes::write_not_permitted;
    }

    std::uint8_t read_blob_c( std::size_t offset, std::size_t read_size, std::uint8_t* out_buffer, std::size_t& out_size ) const;
    std::uint8_t read_blob_v( std::size_t offset, std::size_t read_size, std::uint8_t* out_buffer, std::size_t& out_size ) volatile;
    std::uint8_t read_blob_vc( std::size_t offset, std::size_t read_size, std::uint8_t* out_buffer, std::size_t& out_size ) const volatile;
} handler_instance;

static constexpr char server_name[] = "MyBLEServer";
using blinky_server = bluetoe::server<
	bluetoe::server_name<server_name>,
    bluetoe::service<
        bluetoe::service_uuid< 0x8C8B4094, 0x0DE2, 0x499F, 0xA28A, 0x4EED5BC73CA9 >,
        bluetoe::characteristic<
            bluetoe::free_read_blob_handler< &read_blob_handler >,
            bluetoe::free_write_blob_handler< &write_blob_handler >,
			bluetoe::indicate
        >,
        bluetoe::characteristic<
            bluetoe::free_read_handler< &read_handler >,
            bluetoe::free_raw_write_handler< &write_handler >,
			bluetoe::indicate
        >,
        bluetoe::characteristic<
            bluetoe::free_read_blob_handler< &static_handler::read >,
            bluetoe::free_write_blob_handler< &static_handler::write >,
			bluetoe::indicate
        >,
        bluetoe::characteristic<
            bluetoe::read_blob_handler< handler, handler_instance, &handler::read_blob >,
            bluetoe::write_blob_handler< handler, handler_instance, &handler::write_blob >,
			bluetoe::indicate
        >
    >
> ;
#endif

auto gatt = blinky_server();

blinky_server::connection_data conn(23);

template<typename T>
void send_packet(T packet) {
	while(!esp_vhci_host_check_send_available()) ;
	esp_vhci_host_send_packet((uint8_t*)&packet, sizeof(T));
}

esp_vhci_host_callback_t cb = {
		/*!< callback used to notify that the host can send packet to controller */
		[]() {
		},
		/*!< callback used to notify that the controller has a packet to send to the host*/
		[](uint8_t* data, uint16_t len)->int {
			if(data[0] == HCIT_TYPE_EVENT) {
			}
			else if(data[0] == HCIT_TYPE_ACL_DATA) {
#pragma pack(1)
				struct {
					uint8_t packet_type = HCIT_TYPE_ACL_DATA;

					uint16_t handle_flags = 0;

					uint16_t len;
					uint16_t pdu_len;
					uint16_t ch_id = 0x0004;
					uint8_t pdu[256];
				} buffer;
#pragma pack()
				size_t size=512;
				auto header = *(DataPacketHeader*)(data);
				gatt.l2cap_input(&(data[9]),header.pdu_len,&(buffer.pdu[0]),size,conn);
				buffer.pdu_len = size;
				buffer.len = size + 4;

				while(!esp_vhci_host_check_send_available()) yield();
				esp_vhci_host_send_packet((uint8_t*)&buffer, size+9);
#if 0
				Serial.println("client request");
				for(auto i = 9; i < len; i++) {
					Serial.printf("%02x", data[i]);
				}
				Serial.println();
				Serial.println("server response");
				for(auto i = 0; i < size; i++) {
					Serial.printf("%02x", buffer.pdu[i]);
				}
				Serial.println('\n');
#endif
			}
			return 0;
		}
};

void setup()
{
	auto time_begin = micros();

	Serial.begin(921600);
	Serial.println();
	nvs_flash_init();
	esp_vhci_host_register_callback(&cb);
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
	esp_bt_controller_init(&bt_cfg);
	esp_bt_controller_enable(ESP_BT_MODE_BLE);


	auto adv_size = gatt.advertising_data(&(hci_ble_write_adv_data_cmd.adv_data[0]), 31);
	hci_ble_write_adv_data_cmd.adv_data_len = adv_size;

	Serial.println("Reset Controller");
	send_packet(hci_reset_cmd);
	send_packet(hci_ble_write_adv_params_cmd);
	send_packet(hci_ble_write_adv_data_cmd);
	send_packet(hci_ble_write_adv_enable_cmd);

	auto time_end = micros() - time_begin;
	Serial.printf("time elapsed %lu us \n", time_end);
}

void loop()
{
}
