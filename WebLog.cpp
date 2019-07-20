
#include "WebLog.h"
#include "nettime.h"
#include <Adafruit_SSD1306.h>
extern "C" {
#include "user_interface.h"
}


WebLog::WebLog(int isize) {
	size = isize;
	entries = new WebEntry[size];
}

void WebLog::Clear() {
	headPtr = 0;
	tailPtr = 0;
	for (int i = 0; i < size; i++) {
		entries[i].Clear();
	}
}

void WebLog::It(float time, const char *message) {
	//Add it to serial output.....
	Serial.printf("%2.2f:%s\n", time, message);
	entries[headPtr].timeOfDay = time;
	strncpy(entries[headPtr].message, message, 80);
	headPtr++;
	headPtr = headPtr%size;
	if (headPtr == tailPtr) {
		tailPtr++;
		tailPtr = tailPtr % size;
	}
}

void WebLog::println(const char *msg) {
	It(netTime.getHourFloat(), msg);
}

void WebLog::Print(void(*pf)(const char *)) {
	char buffer[20];
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("<p>Run time:");
	sprintf(buffer, "%6.2f", netTime.getRunTimeHours());
	pf(buffer);
	pf("</p>");

	pf("<table class=\"table table-hover table-bordered\">");
	pf("<thead><tr><th>Time</th><th>Message</th></tr></thead>");
	pf("<tbody>");
	int i = tailPtr;
	while (i != headPtr) {
		pf("<tr><td>");
		sprintf(buffer, "%2.2f", entries[i].timeOfDay);
		pf(buffer);
		pf("</td><td>");
		pf(entries[i].message);  //Gotta watch the //\\ special chars here
		pf("</td></tr>");
		i++;
		i = i%size;
	}
	pf("</tbody>");
	pf("</table>");
}

int WebLog::FreeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void WebLog::PrintReverse(void(*pf)(const char *)) {
	char buffer[20];
	pf("<div class=\"well\">\n");
	sprintf(buffer, "C++ Version: %ld", __cplusplus);
	pf(buffer);
	sprintf(buffer, "Free ram (bytes):%ld", system_get_free_heap_size());
	pf(buffer);
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("System Run Time: ");
		sprintf(buffer, "%6.2f", netTime.getRunTimeHours());
	pf(buffer);
	pf("</div>\n");
	pf("<table class=\"table table-hover table-bordered\">");
	pf("<thead><tr><th>Time</th><th>Message</th></tr></thead>");
	pf("<tbody>");
	int i = headPtr;
	for (;;) {
		if (i == tailPtr) break;
		i--;
		if (i<0) i += size;
		i = i%size;

		pf("<tr><td>");
		sprintf(buffer, "%2.2f", entries[i].timeOfDay);
		pf(buffer);
		pf("</td><td>");
		pf(entries[i].message);  //Gotta watch the //\\ special chars here
		pf("</td></tr>\n");
	}
	pf("</tbody>");
	pf("</table>");
}



WebLog webLog = WebLog(40);