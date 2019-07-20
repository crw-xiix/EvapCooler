#include "nettime.h"
#include <Arduino.h>
#include <WiFiUdp.h>

#include "WebLog.h"
#include <time.h>

/*


*/

NetTime::NetTime() {
	//Don't put anything in ctor that regs another class......
	secsPastMid = 0;
	lastTime = millis();
	secsSinceLastTimeUpdate = 0;  //Trigger it to look
	runTime = 0;
}

void NetTime::Init(long iTZ) {
	this->timeZone = iTZ;
	if (!udp.begin(localPort)) {
		webLog.println("Can't open port!");
	}
	webLog.It(-1,"NetTime::Initializing");
}
	
void NetTime::setTime(int h, int m, int s) {
	secsSinceLastTimeUpdate = 0;
	//For safety
	secsPastMid = ((unsigned long)h) * ((unsigned long )3600);
	secsPastMid += m * 60;
	secsPastMid += s;
}

void NetTime::setTimeSecs(int s) {
	secsSinceLastTimeUpdate = 0;
	secsPastMid = s;
	
	
}

long NetTime::getTimeSec() {
	return secsPastMid;
}

float NetTime::getHourFloat() {
	return secsPastMid / 3600.0f;
}

float NetTime::getRunTimeHours() {
	return runTime / 3600.0f;
}

TimeODay NetTime::getTime() {
	TimeODay time;
	time.second = (secsPastMid % 60);
	time.minute = (secsPastMid % 3600) / 60;
	time.hour = secsPastMid / 24;
	return time;
}

//We are just going to assume we get called once a second...
void NetTime::process() {
	if ((millis() - lastTime) > 1000) {
		//for rollover
		lastTime += ((unsigned long)1000);
		secsPastMid++;
		secsSinceLastTimeUpdate++;
		runTime++;
		if ((secsPastMid >= 86400ul)||(triggerReload)) {
			triggerReload = false;
			invalidTime = true;
			secsPastMid = secsPastMid % 86400ul;
			if (funcMidnight != NULL) funcMidnight();
			//Trigger a time update if it's there, otherwise continue
			//With whatever we had before.
			first = true;
		}
	}
	checkTimeRequest();
}

// On the hour for now...
bool NetTime::needNewTime() {
	if (secsSinceLastTimeUpdate > (3600)) return true; // On the hour for now...
	return false;
}

void NetTime::sendTimeRequest() {
	//get a random server from the pool
	WiFi.hostByName(ntpServerName, timeServerIP);
	lastSendTime = millis();
	sendNTPpacket(timeServerIP); // send an NTP packet to a time server
	webLog.It(getHourFloat(), "NetTime::Sending time request.");
								 // wait to see if a reply is available
//	Serial.println(timeServerIP);
}
void NetTime::checkTimeRequest() {
	char buffer[80];
	if (needNewTime() || first) {
		if (((millis() - lastSendTime) > 20000) || (first)) {
			first = false;
			sendTimeRequest();
			lastSendTime = millis();
		}
	}
	int cb = udp.parsePacket();
	if (!cb) {
		return;
	}
	else {
		secsSinceLastTimeUpdate = 0;
		//Serial.print("Packet received, length=");
//		Serial.println(cb);
		// We've received a packet, read the data from it
		udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

												 //the timestamp starts at byte 40 of the received packet and is four bytes,
												 // or two words, long. First, esxtract the two words:

		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900):
		unsigned long secsSince1900 = highWord << 16 | lowWord;
		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		const unsigned long seventyYears = 2208988800UL;
		// subtract seventy years:

		unsigned long epoch = secsSince1900 - seventyYears;
		
		//Adjust for Time Zone

		
		epoch += (timeZone * (3600l));

		setTimeSecs(epoch % 86400ul);
		
		int hour, min, sec;
		hour = (epoch % 86400ul) / 3600; //
		min = (epoch % 3600ul) / 60;       
		sec = (epoch % 60ul);

		//Figure out a better way to do this, this feels so 1970
		time_t t = epoch;
		tm *timeResult;
		timeResult = gmtime(&t);

		month = timeResult->tm_mon+1;
		day = timeResult->tm_mday;
		year = timeResult->tm_year+1900;

		//This way, if we want to show the time, we can......
		setTimeSecs(epoch % 86400ul);

		if (funcTimeCalc != NULL) funcTimeCalc();

		
		if (invalidTime) {
			invalidTime = false;
			//Fire off a function.......
			if (funcTimeValid != NULL) funcTimeValid();
		}
	}
	// wait ten seconds before asking for the time again
}


// send an NTP request to the time server at the given address
unsigned long NetTime::sendNTPpacket(IPAddress& address)
{
//	Serial.println("sending NTP packet...");
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
							 // 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();

}

NetTime netTime = NetTime();