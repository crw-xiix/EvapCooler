#include "RemoteSensor.h"
#include "WebLog.h"



RemoteSensor::RemoteSensor():udp() {
	for (int i = 0; i < sz; i++) {
		values[i] = 0.0f;
		updated[i] = false;
	}
}

void RemoteSensor::begin(int port) {
	udp.begin(port);
}

void RemoteSensor::process() {
	char buffer[80];
	float value;
	int index;
	int cb = udp.parsePacket();
	if (!cb) {
		return;
	}
	udp.readBytesUntil('\n', (uint8_t*)buffer, 80);
	udp.flush();
	sscanf(buffer, "%d,%f", &index, &value);

	if ((index >= 0) && (index < sz))
	{
		values[index] = value;

	}
}

bool RemoteSensor::changed(int val) {
	val = val % sz;
	if (updated[val]) {
		updated[val] = false;
		return true;
	}
	return false;
}
float RemoteSensor::getValue(int val) {
	val = val % sz;
	return values[val];
}


