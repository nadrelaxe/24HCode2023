#ifndef MAILBOX_HPP
#define MAILBOX_HPP

#include <CarCtrl.hpp>
#include <WiFiUdp.h>

class Mailbox {
	private:
		typedef std::array<uint8_t, 256> packet_t;
		typedef void(Mailbox::*handler_t)();
		struct handler_info {
			uint8_t id;
			uint8_t len;
			uint8_t min_lvl;
			handler_t handler;
		};

		CarCtrl & _car;
		WiFiUDP _udp;
		WiFiClient _tcp_link;

		unsigned long _pilot_alive = 0;
		uint8_t _alive_protect_step = 0;

		std::array<uint8_t, 6> _password[3] = {
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		};

		packet_t::iterator _packet_head;
		IPAddress _cur_sender;
		bool msg_arg_bool();
		uint8_t msg_arg_u8();
		int16_t msg_arg_i16();
		uint16_t msg_arg_u16();
		template<typename T> void msg_arg_copy(uint8_t nb, T it);

		void readUdp();
		void readTcp();
		void processPacket(packet_t packet, uint8_t len);

		void on_msg_open_tcp_link();

		void on_msg_engine_on();
		void on_msg_pilot();
		void on_msg_headlights();

		void on_msg_save_config();
		void on_msg_change_pass_lvl1();
		void on_msg_change_pass_lvl2();
		void on_msg_change_pass_lvl3();
		void on_msg_change_name();
		void on_msg_change_trims();

		void on_msg_limit_speed();
		void on_msg_invert_steering();
		void on_msg_invert_throttle();
		void on_msg_set_color();

		static constexpr const std::array handlers {
			handler_info{0x01, 2, 2, &Mailbox::on_msg_open_tcp_link},

			handler_info{0x10, 1, 1, &Mailbox::on_msg_engine_on},
			handler_info{0x11, 4, 1, &Mailbox::on_msg_pilot},
			handler_info{0x12, 2, 1, &Mailbox::on_msg_headlights},

			handler_info{0x20, 0, 3, &Mailbox::on_msg_save_config},
			handler_info{0x21, 6, 1, &Mailbox::on_msg_change_pass_lvl1},
			handler_info{0x22, 6, 2, &Mailbox::on_msg_change_pass_lvl2},
			handler_info{0x23, 6, 3, &Mailbox::on_msg_change_pass_lvl3},
			handler_info{0x24, 17, 3, &Mailbox::on_msg_change_name},
			handler_info{0x25, 6, 3, &Mailbox::on_msg_change_trims},

			handler_info{0x30, 4, 2, &Mailbox::on_msg_limit_speed},
			handler_info{0x31, 1, 2, &Mailbox::on_msg_invert_steering},
			handler_info{0x32, 1, 2, &Mailbox::on_msg_invert_throttle},
			handler_info{0x33, 3, 2, &Mailbox::on_msg_set_color},
		};

	public:
		Mailbox(CarCtrl & car) : _car(car) {}
		void init();
		void loop();
};

#endif
