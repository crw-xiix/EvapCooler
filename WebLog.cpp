
#include "WebLog.h"
#include "nettime.h"

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
/*
int i = headPtr;
for (;;) {
if (i == tailPtr) break;
i--;
if (i<0) i+=size;
i = i%size;

//pf("<tr><td>");
sprintf(buffer, "%02d:%2.2f:%s", i,entries[i].timeOfDay,entries[i].message);
pf(buffer);

//pf("</td><td>");
//pf(entries[i].message);  //Gotta watch the //\\ special chars here
//pf("</td></tr>");

}
*/
void WebLog::PrintReverse(void(*pf)(const char *)) {
	char buffer[20];
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
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
		pf("</td></tr>");
	}
	pf("</tbody>");
	pf("</table>");
}



WebLog webLog = WebLog(40);