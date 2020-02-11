#ifndef NODE_HPP
#define NODE_HPP

#include "predicate.hpp"

class node {
public:
	vector<int> input, need_update;
	int power, delay;
	virtual bool update(int power);
	virtual bool update(int power, int side);
};

class wire : public node {
public:
	wire();
	bool update(int power);
};

class torch : public node {
public:
	torch();
	bool update(int power);
};

class repeater : public node {
public:
	int lock;
	repeater(int delay);
	bool update(int power, int side);
};

class comparator : public node {
public:
	comparator();
	bool update(int power, int side);
};

class lever : public node {
public:
	lever();
	bool update(int power);
	bool set_as(int power);
};

class lamp : public node {
public:
	lamp();
	bool update(int power);
};

#endif
