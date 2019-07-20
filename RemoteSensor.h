#pragma once
#include <WiFiUdp.h>

class RemoteSensor {
private:
	static const int sz = 16;
	float values[sz];   //16 values.
	bool updated[sz];
	WiFiUDP udp;
public:
	RemoteSensor();
	void begin(int port);
	void process();
	bool changed(int val);
	float getValue(int val);
};
