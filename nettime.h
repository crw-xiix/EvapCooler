#pragma once
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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
	bool first = true;
	unsigned long lastTime = 0;
	long secsPastMid = 0;
	long secsSinceLastTimeUpdate = 0;
	void sendTimeRequest();
	void checkTimeRequest();
	long timeZone = 0;

public:
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

	float getHourFloat();// 0-24.0
	void(*GotNewTime)() = NULL;


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
	byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
	unsigned long lastSendTime = 0;
	unsigned long sendNTPpacket(IPAddress& address);
};

extern NetTime netTime;



