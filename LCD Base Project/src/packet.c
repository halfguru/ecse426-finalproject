#include "packet.h"

uint8_t control_bit = 0;

void makeLCD2MotorPkt(uint8_t *pkt, uint8_t keypad_val, uint8_t state/*, uint8_t control*/) {
	*pkt = 0x00;
	
	*pkt |= keypad_val;
	*pkt |= (state << 5);
	*pkt |= (control_bit << 6);
	control_bit = (control_bit + 1) % 2;
}

uint8_t makeCallbackPkt(uint8_t *pkt, int8_t x, int8_t y, uint8_t fail) {
	if (fail) {
		*pkt = 0x00;
	} else {
		if ((x < -7 || x > 7) || (y < 6 || y > 12)) {
			return 0;
		}
		int8_t pkt_x = x + 7;
		uint8_t pkt_y = y - 6;
		*pkt = 0x00;
		*pkt = *pkt | (pkt_x << 4);
		*pkt = *pkt | pkt_y;
	}
	return 1;
}
