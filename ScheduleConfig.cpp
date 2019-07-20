
#include "ScheduleConfig.h"
#include <EEPROM.h>
#include "nettime.h"
#include "8266IO.h"



char DevicePassword[32];

ScheduleConfig ScheduleConfig::LoadFromEEPROM() {
	bool fail = false;
	uint8_t ecrc = 0;
	int esize;
	ScheduleConfig sc;
	int size = sizeof(ScheduleConfig) + 8;
	
	EEPROM.begin(size);
	EEPROM.get(0, sc);
	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	EEPROM.get(sizeof(ScheduleConfig) + 1, esize);
	EEPROM.end();
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	

	if (crc != ecrc) {
		webLog.println("CRC Error, Invalid EEPROM DATA");
		sc.ResetDefault();
	}
	if (esize != sizeof(ScheduleConfig)) {
		webLog.println("Size mismatch, Invalid EEPROM DATA");
		sc.ResetDefault();
	}
	webLog.println("Loaded EEPROM DATA");
	return sc;
}

bool ScheduleConfig::SaveToEEPROM() {
	int size = sizeof(ScheduleConfig) + 8;
	int esize = sizeof(ScheduleConfig);

	uint8_t ecrc = 0;
	ScheduleConfig sc = *this;
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	EEPROM.begin(size);
	EEPROM.put(0, sc);
	EEPROM.put(sizeof(ScheduleConfig), crc);
	EEPROM.put(sizeof(ScheduleConfig) + 1, esize);
	EEPROM.commit();

	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	if (crc != ecrc) {
		webLog.println("On Save: Invalid EEPROM DATA");
	}
	else {
		webLog.println("Verified EEPROM data");
	}
	EEPROM.end();
}

//Only stores a pointer to the value.....
void ScheduleConfig::SetPassword(const char *val) {
	strncpy(DevicePassword, val,32);
	DevicePassword[31] = 0;
}

void ScheduleConfig::OutputJson(Stream* dev) {
	dev->printf("{\n");
	dev->printf("\"Title\": \"%s\",\n", sConfig.Title);
	dev->printf("\"Version\": \"%s\",\n", sConfig.Version);
	dev->printf("\"DST\": %s,\n", sConfig.DST ? "true" : "false");
	dev->printf("\"DayTemp\": %.1f,\n", DayTemp);
	dev->printf("\"NightMax\": %.1f,\n", NightMax);
	dev->printf("\"NightMin\": %.1f,\n", NightMin);
	dev->printf("\"FanMode\" : %d,\n", (int)FanMode);
	dev->printf("\"PumpMode\" : %d,\n", (int)PumpMode);
	dev->printf("\"VacationMode\" : %d\n", (int)VacationMode);
	dev->println("}");
}

void ScheduleConfig::ResetDefault() {
	DST = false;
	strcpy(Title, "New Remote Action Unit");
	strcpy(Version, "Enter something here");
	DayTemp = 78.0;
	NightMax = 76.0;
	NightMin = 65.0;
	FanMode = TriState::Automatic;
	PumpMode = TriState::Automatic;
	VacationMode = false;
}


float getFloatDefault(const char *src, float dft) {
	float v;
	if (sscanf(src, "%f", &v) > 0) return v;
	return dft;
}

TriState getTriStateDefault(const char *src, TriState val) {
	int v;
	if (sscanf(src, "%d", &v) > 0) if ((v>=0)&&(v<=2)) return TriState(v);
	return val;
}

bool getBoolDefault(const char *src, bool val) {
	int v;
	if (sscanf(src, "%d", &v) > 0) {
		if (v == 0) return false;
		if (v == 1) return true;
	}
	if (strstr(src, "true")) return true;
	if (strstr(src, "false")) return false;
	return val;
}

bool isPresent(const char *search, const char *phrase, const char *before) {
	const char *tmp = strstr(search, phrase);
	if (tmp == NULL) return false;
	if (tmp >= before) return false;
	return true;
}

//Returns true if good data, false to block
bool ScheduleConfig::HandlePostData(WiFiClient& client) {
	char password[80];
	password[0] = 0;

	//read till 2 line feeds
	char buffer[120];
	char decoded[100];
	const char *ptr;
	int lastLen = 119;
	bool okay = true;
	bool firstTask = true;
	
	while (true) {
		if (!client.connected()) break;
		bool val = readBytesUntil(client,buffer, '\n', 120,500);
		Serial.print(buffer);
		//Something failed, too long, timeout, something, loop.
		if (!val) continue;
		
		int v;
		ptr = strstr(buffer, "=");
		if (!ptr) {
			if (strstr(buffer, "END") == buffer) break;
			continue;
		}
		//End it at the comma
		if (char *pcomma = strstr(buffer, ",")) *pcomma = 0;

		ptr++;
		//Password first......
		

		if (isPresent(buffer, "Password=", ptr)) {
			strncpy(password, ptr, 32);
			webLog.println(password);
			if (strcmp(password, DevicePassword) != 0) break;

		}

		if (password[0] == 0) continue;
		if (isPresent(buffer, "Title", ptr)) strncpy(sConfig.Title, ptr, sizeof(sConfig.Title));
		if (isPresent(buffer, "Version", ptr)) strncpy(sConfig.Version, ptr, sizeof(sConfig.Version));
		if (isPresent(buffer, "DST", ptr)) if (sscanf(ptr, "%d", &v) > 0) sConfig.DST = (v != 0);
		if (isPresent(buffer, "DayTemp", ptr)) DayTemp = getFloatDefault(ptr, DayTemp);
		if (isPresent(buffer, "NightMax", ptr)) NightMax = getFloatDefault(ptr, NightMax);
		if (isPresent(buffer, "NightMin", ptr)) NightMin = getFloatDefault(ptr, NightMin);
		if (isPresent(buffer, "DayTemp", ptr)) DayTemp = getFloatDefault(ptr, DayTemp);
		if (isPresent(buffer, "FanMode", ptr)) FanMode = getTriStateDefault(ptr, FanMode);
		if (isPresent(buffer, "PumpMode", ptr))  PumpMode = getTriStateDefault(ptr, PumpMode);
		if (isPresent(buffer, "VacationMode", ptr)) VacationMode = getBoolDefault(ptr, VacationMode);
	}
	client.flush();
	if (strcmp(password, DevicePassword) != 0) {
		sprintf(buffer, "Actual: %s  Tried: %s",  DevicePassword, password);
		webLog.println("Invalid Password");
		webLog.println(buffer);
		
		return false;
	}
	else {
		sConfig.SaveToEEPROM();
		webLog.println("EEPROM Saved");
		return true;

	}
	
}

ScheduleConfig sConfig = ScheduleConfig();