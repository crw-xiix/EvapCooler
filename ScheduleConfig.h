#pragma once
#ifndef __ScheduleConfig_H__ 
#define __ScheduleConfig_H__ 1


//#include "SpecTask.h"
#include "crc.h"
#include <stdint.h>
#include "WebLog.h"
#include "ConfigTypes.h"
#include <ESP8266WiFi.h>

class ScheduleConfig {
public:
	TriState FanMode;
	TriState PumpMode;
	bool VacationMode;
	float DayTemp;
	float NightMax;
	float NightMin;
	//Title of the App
	char Title[80];
	//Special notes about the version
	char Version[80];
	int  DST = false;
	//These get updated by the sunrise code and saved when config is changed.
	//If no time server, it can be adjusted manually.
	float Sunrise = 6.0f;
	float Sunset = 18.0f;
	float extraFloats[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t extraBytes[8] = { 0,0,0,0,0,0,0,0 };
	int	extraInts[8] = { 0,0,0,0,0,0,0,0 };
	void SetPassword(const char *val);
	ScheduleConfig LoadFromEEPROM();
	bool SaveToEEPROM();
	void ResetDefault();
	void OutputJson(Stream* dev);
	bool HandlePostData(WiFiClient& client);

};

extern ScheduleConfig sConfig;

#endif