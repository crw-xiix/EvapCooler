#include <ESP8266WiFi.h>

//Reads until timeout or terminator or buffer full.  Returns true if all succeed, false if timeout or overrun. It always terminates with /0
//terminator charaacter IS INCLUDED in the string
bool readBytesUntil(WiFiClient& client, char* dest, char terminator, int len, unsigned long timeout_ms) {
	unsigned long start = millis();
	int pos = 0;
	while ((millis() - start) < timeout_ms) {
		if (!client.connected()) {
			dest[0] = 0;
			break;
		}
		if (client.available()) {
			char c = client.read();
			dest[pos++] = c;
			if (c == terminator) {
				dest[pos] = 0;
				return true;
			}
			if (pos >= len) {
				dest[len - 1] = 0;
				return false;
			}
		}
		yield();
	}
	return false;
}