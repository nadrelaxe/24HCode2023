#include <Mailbox.hpp>

void Mailbox::init() {
	_password[2] = _car.configAdminPass();
	_udp.begin(4210);
}

void Mailbox::loop() {
	if (_udp.parsePacket() > 0) readUdp();
	if (_tcp_link.available() > 0) readTcp();

	unsigned long pilot_idle = millis() - _pilot_alive;
	if (pilot_idle >= 200 && _alive_protect_step < 1) {
		_alive_protect_step = 1;
		_car.pilot(0, 0);
	}
	if (pilot_idle >= 500 && _alive_protect_step < 2) {
		_alive_protect_step = 2;
		_car.start_engine(false);
	}
}

void Mailbox::readUdp() {
	packet_t packet;
	int len = _udp.read(packet.data(), packet.size());
	if (len <= 0) return;
	_cur_sender = _udp.remoteIP();
	processPacket(packet, len);
}

void Mailbox::readTcp() {
	packet_t packet;
	int len = _tcp_link.read(packet.data(), packet.size());
	if (len <= 0) return;
	_cur_sender = _tcp_link.remoteIP();
	processPacket(packet, len);
}

void Mailbox::processPacket(packet_t packet, uint8_t len) {
	if (len < 5) return;
	if (packet[0] != 'C' || packet[1] != 'I' || packet[2] != 'S') return;
	
	uint8_t pass_lvl = packet[3];
	_packet_head = packet.begin() + 4;
	auto packet_end = packet.begin() + len;
	if (pass_lvl > 3) {
		return;
	} else if (pass_lvl > 0) {
		if (len < 10) return;
		auto & pass = _password[pass_lvl-1];
		if (!std::equal(pass.begin(), pass.end(), _packet_head)) pass_lvl = 0;
		_packet_head += 6;
	} else {
		pass_lvl = (_password[0] == std::array<uint8_t, 6>{0,0,0,0,0,0}) ? 1 : 0;
	}

	while (_packet_head != packet_end) {
		auto it = std::find_if(handlers.begin(), handlers.end(), [this](auto h) {
			return h.id == *_packet_head;
		});
		if (it == handlers.end()) return;
		auto packet_start = _packet_head;
		if (std::distance(_packet_head, packet_end) < it->len + 1) return;
		if (pass_lvl >= it->min_lvl) {
			_packet_head++;
			std::invoke(it->handler, this);
		}
		_packet_head = packet_start + it->len + 1;
	}
}

bool Mailbox::msg_arg_bool() {
	return *(_packet_head++) != 0;
}

uint8_t Mailbox::msg_arg_u8() {
	return *(_packet_head++);
}

int16_t Mailbox::msg_arg_i16() { 
	const int16_t msb = *(_packet_head++);
	return msb << 8 | *(_packet_head++);
}

uint16_t Mailbox::msg_arg_u16() {
	const uint16_t msb = *(_packet_head++);
	return msb << 8 | *(_packet_head++);
}

template<typename T> void Mailbox::msg_arg_copy(uint8_t nb, T it) {
	std::copy(_packet_head, _packet_head+nb, it);
}

void Mailbox::on_msg_open_tcp_link() {
	const uint16_t port = msg_arg_u16();
	if (_tcp_link.connected()) _tcp_link.stop();
	_tcp_link.connect(_cur_sender, port);
}

void Mailbox::on_msg_engine_on() {
	const bool on = msg_arg_bool();
	_car.start_engine(on);
}

void Mailbox::on_msg_pilot() {
	const int16_t throttle = msg_arg_i16();
	const int16_t steering = msg_arg_i16();
	_car.pilot(throttle, steering);
	_pilot_alive = millis();
	_alive_protect_step = 0;
}

void Mailbox::on_msg_headlights() {
	const int16_t value = msg_arg_i16();
	_car.setHeadlights(value);
}

void Mailbox::on_msg_save_config() {
	_car.saveConfig();
}

void Mailbox::on_msg_change_pass_lvl1() {
	msg_arg_copy(6, _password[0].begin());
}

void Mailbox::on_msg_change_pass_lvl2() {
	msg_arg_copy(6, _password[1].begin());
}

void Mailbox::on_msg_change_pass_lvl3() {
	msg_arg_copy(6, _password[2].begin());
	_car.setConfigAdminPass(_password[2]);
}

void Mailbox::on_msg_change_name() {
	char name[18];
	msg_arg_copy(17, name);
	name[17] = 0;
	_car.setConfigName(String(name));
}

void Mailbox::on_msg_change_trims() {
	const int16_t stering_trim = msg_arg_i16();
	_car.setConfigSteeringTrim(stering_trim);
	const uint16_t fw = msg_arg_u16();
	const uint16_t bw = msg_arg_u16();
	_car.setConfigThrottleStart(fw, bw);
}

void Mailbox::on_msg_limit_speed() {
	const int16_t pos = msg_arg_i16();
	const int16_t neg = msg_arg_i16();
	_car.limitSpeed(pos, neg);
}

void Mailbox::on_msg_invert_steering() {
	const bool on = msg_arg_bool();
	_car.invertSteering(on);
}

void Mailbox::on_msg_invert_throttle() {
	const bool on = msg_arg_bool();
	_car.invertThrottle(on);
}

void Mailbox::on_msg_set_color() {
	std::array<uint8_t, 3> color;
	msg_arg_copy(3, color.begin());
	_car.setColor(color);
}

