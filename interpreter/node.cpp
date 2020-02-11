#include "node.hpp"

bool node::update(int power) {
	return false;
}

bool node::update(int power, int side) {
	return false;
}

wire::wire() {
	this->delay = 0;
}

bool wire::update(int power) {
	if (this->power == power) {
		return false;
	}
	this->power = power;
	return true;
}

torch::torch() {
	this->delay = 1;
}

bool torch::update(int power) {
	printf("torch update, power = %d\n", power);
	int calculated_power = power ? 0 : 15;
	if (this->power == calculated_power) {
		return false;
	}
	this->power = calculated_power;
	return true;
}

repeater::repeater(int delay) {
	this->delay = delay;
	this->lock = 0;
}

bool repeater::update(int power, int side) {
	if (side) {
		if (lock == side) {
			return false;
		}
		lock = side;
		return true;
	}
	int calculated_power = power ? 15 : 0;
	if (this->power == calculated_power && lock == side) {
		return false;
	}
	this->lock = side;
	this->power = calculated_power;
	return true;
}

comparator::comparator() {
	this->delay = 1;
}

bool comparator::update(int power, int side) {
	int calculated_power = power >= side ? power : 0;
	if (this->power == calculated_power) {
		return false;
	}
	this->power = calculated_power;
	return true;
}

lever::lever() {
	this->delay = 0;
}

bool lever::update(int power) {
	return false;
}

bool lever::set_as(int power) {
	power = power ? 15 : 0;
	if (this->power == power) {
		return false;
	}
	this->power = power;
	return true;
}

lamp::lamp() {
	this->delay = 0;
}

bool lamp::update(int power) {
	this->power = power;
	return false;
}

