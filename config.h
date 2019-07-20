#pragma once
#include <stdint.h>

struct CoolerConfig {
public: 
	//For display on screen
	char SiteName[80];
	//URL to location of image, can be local if supported
	char ImageBkg[256];
	//TimeZone without daylight savings
	uint8_t TimeZone = 0;  
	bool DaylightSavings = false;
	float Lat = 0.0f;
	float Long = 0.0f;

	//Then our task list



};

