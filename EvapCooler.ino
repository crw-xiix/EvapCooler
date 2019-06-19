

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
#include <ESP8266WiFi.h>
#include <time.h>
#include <ArduinoOTA.h>


//Water level
#include "Ultrasonic.h"
//Temperature
#include "mcp.h"
//Wensite
#include "output.h"
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

MCP9808 mcp = MCP9808();

WiFiServer server(80);

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

double sunRise = 6;
double sunSet = 6;

void dummy() {
	
	SunSet sun = SunSet();
	sun.setPosition(DEVICE_LAT, DEVICE_LON, DEVICE_TZ);
	sun.setCurrentDate(netTime.year, netTime.month, netTime.day);
	sunRise = sun.calcSunrise()/60;
	sunSet = sun.calcSunset()/60;
	char buffer[80];
	snprintf(buffer, 80, "Sunrise:%f Sunset:%f", sunRise, sunSet);

	webLog.println(buffer);
}

EvapCooler cooler(PIN_PUMP_OUT, PIN_FAN_OUT, PIN_THERMOSTATC, PIN_THERMOSTATC);



void setup() {
	String st;
	EEPROM.begin(512);
	Serial.begin(9600);
	delay(100);



	//Test and see if it's valid data.......  This is our signature.......
	webLog.It(-1, "System Startup");
	webLog.It(-1, "Reading EEPROM");
	if ((EEPROM.read(0) == 'C') &&
		(EEPROM.read(1) == 'R') &&
		(EEPROM.read(2) == 'W') &&
		(EEPROM.read(3) == 0)) {
		char str[40];
		//Good EEPROM (hah)
		char myStr[40];

		EEReadStr(4, myStr, 40);
	}
	else {
		webLog.It(-1, "Writing EEPROM data");
		EEWriteStr(0, "CRW", 4);
		EEWriteStr(4, "Testing", 40);
		EEPROM.commit();
	}




	int j[10];
	j[5] = 4;
	int p = 5[j];


	//pinMode(TRIG, OUTPUT);
	pinMode(ledPin, OUTPUT);

	if (!mcp.begin()) {
		webLog.It(-1, "Couldn't find MCP9808!");
	}
	else {
		webLog.It(-1, "Found MCP9808!");
	}

	Wire.setClock(10000);

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);

	WiFi.hostname(wifiHostName);
	st = "Connecting to:";
	st += wifiNetwork;

	webLog.It(-1, st.c_str());

	WiFi.config(ip, gateway, subnet, dns1, dns2);
	WiFi.begin(wifiNetwork, wifiPassword);

	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
	}

	SetupOTA("EvapCooler-ESP8235");


	netTime.GotNewTime =dummy;
	netTime.Init(-8);
	netTime.process();

	// Start the server
	server.begin();
	webLog.println("Server started:");

	webLog.println(WiFi.localIP().toString().c_str());



	//int sz = EEPROM.read(1);
	/*
	Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
	Sdtring subject = "ESP8285 Water monitoring system";
	if(gsender->Subject(subject)->Send("charles@loneaspen.com", "The system has been restarted after a power down.")) {
	Serial.println("Message sent.");
	} else {
	Serial.print("Error sending message: ");
	Serial.println(gsender->getError());
	}
	*/

	//	WindSensor::Init(PIN_WIND_SPEED_IN, 5.0f);

	cooler.reset();


}


float lastWaterLevel = 0;
float lastTemp = 0.0f;

//volatile float lastHz = 0;


WiFiClient *tempClient = NULL;


void loop() {
	//For reprogramming
	ArduinoOTA.handle();
	//Lets us know the program is running.......
	digitalWrite(ledPin, !digitalRead(ledPin));

	netTime.process();

	cooler.process();

	mcp.readSensor();
	lastTemp = mcp.getTemperature_F();

	cooler.setTempC(lastTemp);

	distance = ultrasonic.read();

	float tempWaterLevel = 33.3f - (distance / 2.54f);
	if (tempWaterLevel < 0) tempWaterLevel = 0.0f;
	tempWaterLevel = tempWaterLevel * 165.0f / 32.0f;

	// 25 sample running/rolling average
	lastWaterLevel = (lastWaterLevel * 24.0f + tempWaterLevel) / 25.0f;

	WiFiClient client = server.available();
	int lps = 0;

	while (client == NULL)
	{
		if (lps > 100) return;  //Skip wifi stuff.
		delay(1);
		client = server.available();
		lps++;
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

	// Read the first line of the request
	char request[160];
	size_t bytes = client.readBytesUntil('\r', request, 160); // as readBytes with terminator character
//	String request = client.readStringUntil('\r');
	//Serial.println(request);
	client.flush();
	if (!client.connected()) return;
	// Match the request
	int value = LOW;

	if (strstr(request,"/ToggleLed") != NULL) {
		digitalWrite(ledPin, !digitalRead(ledPin));
		client.println("<br>Updated<br>");
		return;
	}
	if (strstr(request,"/log") != NULL) {
		tempClient = &client;
		webLog.PrintReverse(clientPrint);
		tempClient = NULL;
		return;
	}
	if (strstr(request,"/data.json") != NULL) {
		client.println("{");
		client.printf("\"Temp\": %f,", lastTemp);

		client.printf("\"Time\":%f,\n", netTime.getHourFloat());

		client.printf("\"SunRise\":%lf,\n", sunRise);

		client.printf("\"SunSet\":%lf,\n", sunSet);

		tempClient = &client;
		cooler.statsToJson(&clientPrint);
		tempClient = NULL;

		client.printf("\"Water\" : %f ", lastWaterLevel);

		client.println("}");

		return;
	}

	// Return the response
	tempClient = &client;
	outputSite(&clientPrint);
	tempClient = NULL;
	delay(5);
}


void clientPrint(const char *data) {
	tempClient->println(data);
}

