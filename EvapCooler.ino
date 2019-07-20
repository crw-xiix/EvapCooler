

/*
	Name:       EvapCooler.ino
	Created:	5/15/2019 12:10:52 PM
	Author:     charles-THINK\charles
*/

/*  //Send script
python.exe "C:\Users\charles\AppData\Local\arduino15\packages\esp8266\hardware\esp8266\2.4.2/tools/espota.py" -i "192.168.89.101" -p "8266" "--auth=" -f "o:\newcode\EvapCooler\Release\EvapCooler.ino.bin"
*/



/*
#include <WiFiUdp.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecure.h>
#include <WiFiServer.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <CertStoreBearSSL.h>
#include <BearSSLHelpers.h>
#include <Wire.h>
*/
#include <ESP8266WebServerSecureBearSSL.h>
#include <ESP8266WebServerSecureAxTLS.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <ArduinoOTA.h>
#include <functional>
#include <algorithm>
#include "Blocker.h"

//Water level
#include "Ultrasonic.h"
//Temperature
#include "mcp.h"

//Website
#include "output.h"

//Website Edit
#include "outputEdit.h"


//Input/Output pins
#include "pins.h"
//Wind sensor
#include "wind.h"
//Cooler login
#include "cooler.h"
//Storage
#include "EEPROM.h"
//Time Day and internet time
#include "nettime.h"
//Config options
#include "passwords.h"
//On webpage action log
#include "WebLog.h"
//OTA actions
#include "OTASetup.h"
//Sunrise and Sunset calculations
#include "./astronomical.h"
//Remote Temp gauge
#include "RemoteSensor.h"

//Config
#include "ScheduleConfig.h"

///Readbytes
#include "8266IO.h"


#define ledPin 2

#ifndef _EMAIL_CREDS_
#define _EMAIL_CREDS_
#define CREDS_EMAILBASE64_LOGIN "dasdfsdasasdadfasfasdfa"
#define CREDS_EMAILBASE64_PASSWORD "asqwersdvasdfasdfafdas"
#define CREDS_FROM "asdfasdfaqwerwadfs@gmail.com"
#endif



IPAddress ip(DEVICE_IP); // where .xxx is the desired IP Address
IPAddress gateway(DEVICE_GATEWAY); // set gateway to match your network
IPAddress subnet(DEVICE_SUBNET); // set subnet mask to match your
IPAddress dns1(DEVICE_DNS1);
IPAddress dns2(DEVICE_DNS2);



//Ultrasonic ultrasonic(TRIG, ECHO);
Ultrasonic ultrasonic(PIN_WATER_OUT, PIN_WATER_IN);
float distance;

MCP9808 mcpInside = MCP9808();
MCP9808 mcpOutside = MCP9808();


WiFiServer server(80);

RemoteSensor remoteSensor = RemoteSensor();
Blocker blocker = Blocker();


void EEWriteStr(int dest, char *src, int max) {
	int sp = 0;
	for (int i = 0; i < max; i++) {
		EEPROM.write(dest + i, (uint8_t)src[sp]);
		if (src[sp] != 0) sp++;
	}
}

void EEReadStr(int src, char *dest, int max) {
	for (int i = 0; i < max; i++) {
		char c = EEPROM.read(src + i);
		dest[i] = c;
		if (c == 0) return;
	}
}


bool isTheSunUp() {
	float time = netTime.getHourFloat();
	if ((time >= sConfig.Sunrise) && (time <= sConfig.Sunset)) return true;
	return false;
}

void OnNewTime() {
	
	SunSet sun = SunSet();
	int tz = DEVICE_TZ;
	if (sConfig.DST) tz++;
	sun.setPosition(DEVICE_LAT, DEVICE_LON, tz);
	sun.setCurrentDate(netTime.year, netTime.month, netTime.day);
	sConfig.Sunrise = sun.calcSunrise() / 60;
	sConfig.Sunset = sun.calcSunset() / 60;
	char buffer[80];
	snprintf(buffer, 80, "Date: %d/%d/%d Time: %2.2f Rise:%f Set:%f",
		netTime.month,
		netTime.day,
		netTime.year,
		netTime.getHourFloat(),
		sConfig.Sunrise, 
		sConfig.Sunset);
	webLog.println(buffer);
}

EvapCooler cooler(PIN_PUMP_OUT, PIN_FAN_OUT, PIN_THERMOSTATC, PIN_THERMOSTATC, PIN_RPM_IN);

void OnMidnight() {
	blocker.Clear();
}


void setup() {
	char st[120];
	Serial.begin(115200);
	//Test and see if it's valid data.......  This is our signature.......
	webLog.It(-1, "System Startup");
	webLog.It(-1, "Reading EEPROM");

	sConfig = sConfig.LoadFromEEPROM();
	sConfig.SetPassword(DEVICE_PASSWORD);
	
	pinMode(ledPin, OUTPUT);

	if (!mcpInside.begin()) {
		webLog.It(-1, "MCP9808-Inside Not Found!");
	}
	else {
		webLog.It(-1, "Found MCP9808 Inside!");
	}
	if (!mcpOutside.begin(0x19)) {
		webLog.It(-1, "MCP9808-Outside Not Found!");
	}
	else {
		webLog.It(-1, "Found MCP9808 Outside!");
	}/*
	if (!mcp2ndFloor.begin(0x1a)) {
		webLog.It(-1, "MCP9808-2ndFloor Not Found!");
	}
	else {
		webLog.It(-1, "Found MCP9808 2ndFloor!");
	}*/

	Wire.setClock(10000);

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);

	WiFi.hostname(wifiHostName);
	sprintf(st, "Connecting to: %d", wifiNetwork);
	webLog.It(-1, st);

	WiFi.config(ip, gateway, subnet, dns1, dns2);
	WiFi.begin(wifiNetwork, wifiPassword);

	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
	}
	//Exit with the LED OFF.
	
	SetupOTA("EvapCooler-ESP8235");

	if (sConfig.DST) {
		netTime.Init(DEVICE_TZ+1);
	}
	else {
		netTime.Init(DEVICE_TZ);
	}

	netTime.funcTimeValid = OnMidnight;
	netTime.funcTimeCalc = OnNewTime;
	netTime.process();

	// Start the server
	server.begin();
	webLog.println("Server started:");

	//	WindSensor::Init(PIN_WIND_SPEED_IN, 5.0f);
	cooler.reset();
	remoteSensor.begin(3000);
}

float lastWaterLevel = 0;
float lastTemp = 0.0f;

WiFiClient *tempClient = NULL;


bool blocked = false;



void loop() {
	

	ESP.wdtFeed();
	//For reprogramming
	ArduinoOTA.handle();
	//Lets us know the program is running.......
	digitalWrite(ledPin, !digitalRead(ledPin));

	netTime.process();

	cooler.process();

	mcpInside.readSensor();
	mcpOutside.readSensor();


	

	lastTemp = mcpInside.getTemperature_F();


	cooler.setInTempF(lastTemp);
	cooler.setOutTempF(mcpOutside.getTemperature_F());
	cooler.setSunStatus(isTheSunUp());

	distance = ultrasonic.read();

	float tempWaterLevel = 33.3f - (distance / 2.54f);
	if (tempWaterLevel < 0) tempWaterLevel = 0.0f;
	tempWaterLevel = tempWaterLevel * 165.0f / 32.0f;

	// 25 sample running/rolling average
	lastWaterLevel = (lastWaterLevel * 24.0f + tempWaterLevel) / 25.0f;

	remoteSensor.process();

	//1 is the 2nd floor temp gauge......

	cooler.setRemoteTempF(remoteSensor.getValue(1));
	cooler.setAtticTemp(remoteSensor.getValue(2));


	//Loop out if we lose WiFi
	while (WiFi.status() != WL_CONNECTED) {
		delay(50);
		return;
	}

	/* Everything else after this is the website, so all fucntions have to be above*/

	WiFiClient client = server.available();

	


	int lps = 0;

	

	while (client == NULL)
	{
		delay(10);
		return;  //Skip wifi stuff.
	}

	// Wait until the client sends some data
	//Serial.println("new client");

	while (client.connected() && !client.available())
	{
		delay(1);
		lps++;
		if (lps > 200) return;  //Skip wifi stuff.
	}


	//Bail out, something failed.
	if (!client.connected()) return;

	if (blocker.IsBlocked(client.remoteIP(),netTime.getHourFloat())) blocked = true;



	// Read the first line of the request
	char request[160];
	readBytesUntil(client,request,'\r', 160,250); // as readBytes with terminator character
	client.flush();
	if (!client.connected()) return;
	// Match the request
	int value = LOW;
	
	do {
		if (strstr(request, "/log") != NULL) {
			printHeader(client, "text/html");
			website_log(client);
			break;
		}
		if (strstr(request, "/edit") != NULL) {
			tempClient = &client;
			outputEditSite(&clientPrint);
			tempClient = NULL;
			break;
		}
		if ((strstr(request, "POST /SetConfig"))) {
			if (!blocked) {
				webLog.println("Setting Configuration");
				if (!sConfig.HandlePostData(client)) {
					blocker.BlockIP(client.remoteIP(), netTime.getHourFloat());
					webLog.println("Blocked a user");
				}
				printHeader(client, "text/html");
				client.println("Config Set");
			}
			else {
			}
			break;
		}
		if (strstr(request, "/config.json") != NULL) {
			if (!blocked) {
				printHeader(client, "application/json");
				sConfig.OutputJson(&client);
				break;
			}
		}

		if (strstr(request, "/data.json") != NULL) {
			printHeader(client,"application/json");

			client.println("{");
			client.printf("\"Temp\": %f,\n", lastTemp);

			client.printf("\"Time\":%f,\n", netTime.getHourFloat());

			client.printf("\"SunRise\":%lf,\n", sConfig.Sunrise);

			client.printf("\"SunSet\":%lf,\n", sConfig.Sunset);

			tempClient = &client;
			cooler.statsToJson(&clientPrint);
			tempClient = NULL;

			client.printf("\"Water\" : %3.2f\n", lastWaterLevel);
			client.println("}");
			break;
		}
		// Return the response
		tempClient = &client;
		outputSite(&clientPrint);
		tempClient = NULL;
	} while (0);
	client.flush();
	delay(5);
}



void website_log(WiFiClient c) {
	tempClient = &c;
	printHeader(c, "text/html");
	webLog.PrintReverse([](const char *d) {
		tempClient->println(d);
	});
	tempClient = NULL;
}

void tempSet(const char *st) {
	char buffer[80];
	float v = -40.0f;
	const char *p = strstr(st, "?");
	if (p) {
		
		sscanf(p+1, "%f", &v);
		sprintf(buffer, "Thermostat set to: %.1f F", v);
		webLog.println(buffer);
		webLog.println(st);

		if (v >= 0.0f) {
			cooler.setThermostatTemp(v);
		}
	}
}

///ctype:  application/json : text/html 
void printHeader(WiFiClient client, const char *ctype) {
	//application / json
	client.println(F("HTTP/1.1 200 OK"));
	client.print(F("Content-Type: "));
	client.print(ctype);
	client.println(F("; charset = UTF-8"));
	client.println("");
}


char stackbuf[20];
const char *getVarData(int val) {
	switch (val) {
	case 1: return sConfig.Title;
	case 2: return sConfig.Version;
	case 3: {
		if (blocked) {
			return "<h3>You are blocked from saving</h3>";
		} return "";
		break;
	}
	case 4: {
		sprintf(stackbuf, "%.1f", sConfig.Sunrise);
		return stackbuf;
	}
	case 5: {
		sprintf(stackbuf, "%.1f", sConfig.Sunset);
		return stackbuf;
	}

	default: return "Undefined";
	}
}


void clientPrint(const char *data) {
	int iid = 0;
	//Simple replacement of title, version
	const char *ptr = NULL;
	ptr = strstr(data, "{%");
	if (!tempClient->connected()) return;
	if (ptr) {
		tempClient->write(data, ptr - data);
		ptr += 2;
		if (sscanf(ptr, "%d", &iid) == 1) {
			tempClient->print(getVarData(iid));
		}
		ptr += 2;
		tempClient->println(ptr);
		return;
	}
	tempClient->println(data);
}



