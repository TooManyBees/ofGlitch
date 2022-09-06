#pragma once

namespace arturia {
	signed char relative1(int n) {
		if (n & 0b01000000) {
			return n & 0b00111111;
		}
		else {
			return n | 0b11000000;
		}
	}

	signed char relative2(int n) {
		if (n & 0b01000000) {
			return n | 0b10000000;
		}
		else {
			return n;
		}
	}

	signed char relative3(int n) {
		if (n & 0b00010000) {
			return n & 0b00001111;
		}
		else {
			return n | 0b11110000;
		}
	}
}