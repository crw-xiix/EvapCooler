#pragma once
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <functional>

/*
	NetTime.h:

	Provides a means for keeping track of time in seconds without a RTC.  It also gets the time with a call to time servers via the NTP Pool.

	Author:  Charles Wood
	Date:  2019-6-1

	Call netTime.Init(timeZone) in setup()
	Call netTime.process() in loop() at least once a second.  (more is okay)
	
*/
 

struct TimeODay {
	int hour;
	int minute;
	int second;
};

class NetTime {
private:
	unsigned long runTime = 0;
	unsigned long lastTime = 0;
	long secsPastMid = 0;
	long secsSinceLastTimeUpdate = 0;
	void sendTimeRequest();
	void checkTimeRequest();
	long timeZone = 0;

public:
	bool triggerReload = false;
	bool first = true;
	bool invalidTime = true;
	inline void setTimeZone(int tz) {
		timeZone = tz;
		sendTimeRequest();
	}
	NetTime();
	int month = 1, day = 1, year = 2000;
	void Init(long iTZ);
	void setTime(int h, int m, int s);
	void setTimeSecs(int s);
	long getTimeSec();
	TimeODay getTime();
	//We are just going to assume we get called once a second or so...
	void process();
	bool needNewTime();
	float getRunTimeHours();
	float getHourFloat();// 0-24.0
	//This is called all the time saying we got a time server update, use to calc sunrise and shit.
	std::function<void(void)> funcTimeCalc = NULL;
	//This is called when we go from invalid time to good time, ie: start and after midnight
	std::function<void(void)> funcTimeValid = NULL;
	//This is called to let the program be aware all time are now invalid......
	std::function<void(void)> funcMidnight = NULL;
private:
	//Time stuff
	unsigned int localPort = 2390;      // local port to listen for UDP packets

										/* Don't hardwire the IP address or we won't get the benefits of the pool.
										*  Lookup the IP address for the host name instead */
										//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

	WiFiUDP udp;
	IPAddress timeServerIP; // time.nist.gov NTP server address
	const char* ntpServerName = "time.nist.gov";

	static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
	uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
	unsigned long lastSendTime = 0;
	unsigned long sendNTPpacket(IPAddress& address);
};

extern NetTime netTime;



