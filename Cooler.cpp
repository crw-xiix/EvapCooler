
#include "cooler.h"
#include <ESP8266WiFi.h>
#include "WebLog.h"
#include "nettime.h"

EvapCooler::EvapCooler(int iPumpPin, int iFanPin, int iCoolTPin, int iFanTPin) {
	pumpPin = iPumpPin;
	fanPin = iFanPin;
	thermoCoolPin = iCoolTPin;
	thermoFanPin = iFanTPin;
		
	reset();
}

float EvapCooler::getLastTemp() {	
	return lastTemp;
}

void EvapCooler::setTempC(float val) {
	lastTemp = val;
}

//Act like the power just came on...
void EvapCooler::reset() {
	StartTime = millis();
	lastShutOffTime = millis();
	lastOffTime = millis();
	fanSetting = Automatic;
	pumpSetting = Automatic;
	pumpWaiting = false;
	lastCycleTime = millis();
	lastTemp = 30.0f;
	pinMode(pumpPin, OUTPUT);
	digitalWrite(pumpPin, LOW);
	pinMode(fanPin, OUTPUT);
	digitalWrite(fanPin, LOW);
	pinMode(thermoCoolPin, INPUT);  //Need pull down
	pinMode(thermoFanPin, INPUT);   //Need pull down
}

//These will be processed on next loop
void EvapCooler::overrideFan(TriState val) {
	fanSetting = val;
}

//These will be processed on next loop
void EvapCooler::overridePump(TriState val) {
	pumpSetting = val;
}

//For the webpage
bool EvapCooler::getPumpStatus() {
	return (digitalRead(pumpPin) == HIGH);
}

bool EvapCooler::getFanStatus() {
	return (digitalRead(fanPin) == HIGH);
}

bool EvapCooler::getThermostatStatus() {
	return (digitalRead(thermoCoolPin) == LOW);
}

bool EvapCooler::getThermostatFanOnlyStatus() {
	return (digitalRead(thermoFanPin) == HIGH);
}


unsigned long EvapCooler::getTimeToTurnOn() {
	//Not the best method of intention because of overrides.
	if (pumpWaiting) {

	}
	return 0;
}

void EvapCooler::statsToJson(void(*printFunction)(const char *)) {
	int vacation = false;
	char buffer[80];
	
	sprintf(buffer, "\"Pump\" : %s,",getPumpStatus() ? "true" : "false");
	printFunction(buffer);


	sprintf(buffer, "\"WaitingForPump\" : %s,", pumpWaiting ? "true" : "false");
	printFunction(buffer);


	sprintf(buffer, "\"Fan\" : %s,", getFanStatus() ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"ThermostatCool\" : %s,", getThermostatStatus() ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"LastTemp\" : %f,", lastTemp);
	printFunction(buffer);
}


void EvapCooler::process() {
	char buffer[80];
	bool fan = false;
	bool pump = false;
	unsigned long now;
	bool curPump = getPumpStatus();
	now = millis();
	//skip it;
	if ((now - lastCycleTime) < cycleIntervalTime) return;
	lastCycleTime = now;
	bool tempThermState = getThermostatStatus();
	//Things turning on
	if (tempThermState) {
		pump = true;
		if (!lastThermState) {
			//The switch has been thrown, start the timing
			StartTime = now;
			pumpWaiting = true;
			webLog.println("TStat:On, Timer Started for Fan");
		}
		if (pumpWaiting) {
			if (lastTemp < 70.0f) {
				pumpWaiting = false;
				fan = true;
				webLog.println("Temp is <70F, Fan ON.");
			}
			if (((now - StartTime) > maxDelay)) {
				pumpWaiting = false;
				fan = true;
				webLog.println("Wet cycle finished. Fan ON.");
			}
		}
		else { //pump not waiting
			fan = true;
			pump = curPump;
			if (lastTemp < 69.5f) pump = false;
			if (lastTemp > 70.5f) pump = true;

			if (curPump != pump) {
				snprintf(buffer, 80, "Pump Status: %s", pump ? "On" : "Off");
				webLog.println(buffer);
			}

			
			/*

			if (curPump) {  //If the pump is on........
				if (lastTemp < 70.0f) {
					if (curPump != false) {
						st = "Water Off:";
						st += lastTemp;
						webLog.println(st.c_str());
					}
					pump = false;
				}
			}
			if (!curPump) {
				if (lastTemp > 71.0f) {
					st = "Water On:";
					st += lastTemp;
					webLog.println(st.c_str());
					pump = true;
				}
			}
			*/
		}
	}
	else {  //ThermoCoolPin == LOW
		if (lastThermState) {
			//Just record the shut off time.......
			lastOffTime = now;
			webLog.It(netTime.getHourFloat(),"Fan Off: T-OFF");
		}
	}

	if (fanSetting == Off) fan = false;
	if (pumpSetting == Off) pump = false;

	if (fanSetting == On) fan = true;
	if (pumpSetting == On) pump = true;

	lastThermState = tempThermState;
	digitalWrite(fanPin, fan);
	digitalWrite(pumpPin, pump);
}

