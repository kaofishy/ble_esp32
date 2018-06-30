#ifndef HCI_HPP_
#define HCI_HPP_

#include <tuple>

#include <array>
#include <tuple>
#include <type_traits>

#include <cstdint>
#include <cstdio>

namespace hci {
///
/// HCI Command Type
///
template <uint16_t _ogf, uint16_t _opcode, typename... Parameters> struct command_type {
	static constexpr uint16_t opcode = _opcode | (_ogf<<10);
	static constexpr size_t num_params = sizeof...(Parameters);
	std::tuple<Parameters...> params;

	constexpr command_type(const Parameters... _params)
	: params(std::tuple<Parameters...>(_params...)) {}
	constexpr command_type(const std::tuple<Parameters...> _params)
	: params(_params) {}

	constexpr command_type(){};
};

template <uint16_t _ogf, uint16_t _opcode> struct command_type<_ogf, _opcode> {
	static constexpr uint16_t opcode = _opcode | (_ogf<<10);
	static constexpr size_t num_params = 0;
	std::tuple<> params;
};

using addr_type = std::array<uint8_t, 6>;

///
/// HCI Command Definitions
///

constexpr uint16_t OGF_LINK = 0x01;
constexpr uint16_t OGF_CTRL = 0x03;
constexpr uint16_t OGF_INFO = 0x04;
constexpr uint16_t OGF_STATUS = 0x05;
constexpr uint16_t OGF_LE = 0x08;

// Mandary Commands
using le_add_device_to_white_list = command_type<OGF_LE, 0x0011, uint8_t, addr_type>;
using le_clear_white_list = command_type<OGF_LE, 0x0010>;
using le_read_buffer_size = command_type<OGF_LE, 0x0002>;
using le_read_local_supported_features = command_type<OGF_LE, 0x0003>;
using le_read_supported_states = command_type<OGF_LE, 0x001c>;
using le_read_white_list_size = command_type<OGF_LE, 0x000f>;
using le_remove_device_from_white_list = command_type<OGF_LE, 0x0012, uint8_t, addr_type>;
using le_set_event_mask = command_type<OGF_LE, 0x0001, uint64_t>;
using le_test_end = command_type<OGF_LE, 0x001f>;
using read_bd_addr = command_type<OGF_INFO, 0x0009>;
using read_local_supported_features = command_type<OGF_INFO, 0x0003>;
using read_local_version_information = command_type<OGF_INFO, 0x0001>;
using reset = command_type<OGF_CTRL, 0x0003>;
using read_local_supported_commands = command_type<OGF_INFO, 0x0002>;
using set_event_mask = command_type<OGF_CTRL, 0x0001, uint64_t>;
// C1 Commands
using le_read_advertising_channel_tx_power = command_type<OGF_LE, 0x0007>;
using le_transmitter_test = command_type<OGF_LE, 0x001e, uint8_t, uint8_t, uint8_t>;
using le_set_advertise_enable = command_type<OGF_LE, 0x000a, uint8_t>;
using le_set_advertising_data = command_type<OGF_LE, 0x0008, uint8_t, std::array<uint8_t,31> >;
using le_set_advertising_parameters =
		command_type<OGF_LE, 0x0006, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t,
		addr_type, uint8_t, uint8_t>;
constexpr auto le_set_advertising_parameters_defaults =
		le_set_advertising_parameters(0x0800, 0x0800, 0x00, 0x00, 0x00, {}, 0x07,
				0x00);
// C2 Commands
using le_set_random_address = command_type<OGF_LE, 0x0005, addr_type>;
using le_receiver_test = command_type<OGF_LE, 0x001d, uint8_t>;
using le_set_scan_enable  = command_type<OGF_LE, 0x000c, uint8_t, uint8_t>;
using le_set_scan_parameters  =
		command_type<OGF_LE, 0x000b, uint8_t, uint16_t, uint16_t, uint8_t, uint8_t>;
// C3 Commands
using disconnect = command_type<OGF_LINK, 0x0006, uint16_t, uint8_t>;
using le_connection_update = command_type<OGF_LE, 0x0013, uint16_t, uint16_t,
		uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>;
using le_create_connection_cancel = command_type<OGF_LE, 0x000e>;
using le_create_connection = command_type<OGF_LE, 0x000d, uint16_t, uint16_t, uint8_t,
		uint8_t, addr_type, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>;
using le_read_channel_map = command_type<OGF_LE, 0x0015, uint16_t>;
using le_read_remote_used_features = command_type<OGF_LE, 0x0016, uint16_t>;
using le_set_host_channel_classification = command_type<OGF_LE, 0x0014, std::array<uint8_t, 5> >;
using le_set_scan_response_data = command_type<OGF_LE, 0x0009, uint8_t, std::array<uint8_t, 31> >;
using read_transmit_power_level = command_type<OGF_CTRL, 0x002d, uint16_t, uint8_t>;
using read_remote_version_information = command_type<OGF_LINK, 0x001d, uint16_t>;
using read_rssi = command_type<OGF_STATUS, 0x0005, uint16_t>;
// C4 Commands
using le_encrypt = command_type<OGF_LE, 0x0017, std::array<uint8_t, 16>,
		std::array<uint8_t, 16> >;
using le_long_term_key_request_reply = command_type<OGF_LE, 0x001a, uint16_t,
		std::array<uint8_t, 16> >;
using le_long_term_key_request_negatvie_reply = command_type<OGF_LE, 0x001b, uint16_t>;
using le_rand_command = command_type<OGF_LE, 0x0018>;
using le_start_encryption = command_type<OGF_LE, 0x0019, uint16_t, uint64_t, uint16_t,
		std::array<uint8_t, 16> >;
//// C5 Commands
//using read_buffer_size;
//using read_le_host_support;
//using write_le_host_support;
// C6 Commands
using le_remote_connection_parameter_request_reply = command_type<OGF_LE, 0x0020,
		uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t>;
using le_remote_connection_parameter_request_negative_reply = command_type<OGF_LE, 0x0021,
		uint16_t, uint8_t>;
//// C7 Commands
//using write_authenticated_payload_timeout;
//using read_authenticated_payload_timeout;
//using set_event_mask_page_2;
// C8 Commands
using le_set_data_length = command_type<OGF_LE, 0x0022, uint16_t, uint16_t, uint16_t>;
using le_read_suggested_default_data_length = command_type<OGF_LE, 0x0023>;
using le_write_suggested_default_data_length = command_type<OGF_LE, 0x0024, uint16_t, uint16_t>;
using le_read_maximum_data_length = command_type<OGF_LE, 0x002f>;
//// C9 Commands
//using le_set_random_private_address_timeout;
//using le_set_address_resolution_enable;
//using le_add_device_to_resolving_list;
//using le_remove_device_from_resolving_list;
//using le_clear_resolving_list;
//using le_read_resolving_list_size;
//// C10 Commands
//using le_read_peer_resolvable_address;
//using le_read_local_resolvable_address;
//// Optional Commands
//using host_buffer_size;
//using host_number_of_completed_packets;
//using set_controller_to_host_flow_control;
//using le_read_local_p256_public_key;
//using le_generate_dh_key;



///
/// TMP for iterating through command_type::params
///

template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
element_to_stream(uint8_t * const buffer, const T param) {
	for (auto i = 0; i < sizeof(T); i++) {
		buffer[i] = param >> (i * 8);
	}
}
// assume array if not integral
template <typename T>
typename std::enable_if<!std::is_integral<T>::value>::type
element_to_stream(uint8_t * const buffer, const T param) {
	for (auto i = 0; i < sizeof(T); i++) {
		buffer[i] = param[i];
	}
}

// recursively visit each parameter contained in tuple
template <size_t I> struct visit_tuple {
	template <typename CommandType>
	size_t operator()(uint8_t * const buffer, const CommandType command) {
		size_t size = visit_tuple<I - 1>()(buffer, command);

		element_to_stream(buffer + size, std::get<I - 1>(command.params));

		return size + sizeof(std::get<I - 1>(command.params));
	}
};

template <> struct visit_tuple<1> { // base case for 1 parameters
	template <typename CommandType>
	size_t operator()(uint8_t * const buffer, const CommandType command) {
		element_to_stream(buffer, std::get<0>(command.params));
		return sizeof(std::get<0>(command.params));
	}
};

template <> struct visit_tuple<0> { // base case for 0 parameters
	template <typename CommandType>
	size_t operator()(uint8_t * const buffer, const CommandType command) {
		return 0;
	}
};

///
/// Function to output HCI command packet
///

template <typename CommandType>
size_t write_hci_command(uint8_t * const buffer, size_t size,
		const CommandType command) {
	buffer[0] = command.opcode & 0xff;
	buffer[1] = command.opcode >> 8;
	auto len = visit_tuple<std::tuple_size<decltype(command.params)>::value>()(
			buffer + 3, command);
	buffer[2] = len;
	return len + 3;
}

template <typename CommandType, size_t N>
size_t write_hci_command(uint8_t (&buffer)[N],
		const CommandType command) {
	buffer[0] = command.opcode & 0xff;
	buffer[1] = command.opcode >> 8;
	auto len = visit_tuple<std::tuple_size<decltype(command.params)>::value>()(
			buffer + 3, command);
	buffer[2] = len;
	return len + 3;
}

} // namespace hci

#endif /* HCI_HPP_ */
