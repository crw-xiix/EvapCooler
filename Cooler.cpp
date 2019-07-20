
#include "cooler.h"
#include <ESP8266WiFi.h>
#include "WebLog.h"
#include "nettime.h"
#include "ScheduleConfig.h"



//These are hidden from main program......

volatile unsigned long lastRPMTime = 0;
volatile float fanRPM = 0.0;

void ICACHE_RAM_ATTR ISR_FanRPM() {
	float val;
	unsigned long nowish = millis();
	unsigned long diff = nowish - lastRPMTime;
	if (diff > 0) {
		val = 60000.0 / (nowish - lastRPMTime);
		fanRPM = (fanRPM*9.0 + val) / 10.0;
	}
	lastRPMTime = nowish;
}


EvapCooler::EvapCooler(int iPumpPin, int iFanPin, int iCoolTPin, int iFanTPin, int iRPMpin) {
	pumpPin = iPumpPin;
	fanPin = iFanPin;
	thermoCoolPin = iCoolTPin;
	thermoFanPin = iFanTPin;
	rpmPin = iRPMpin;
	attachInterrupt(digitalPinToInterrupt(rpmPin), ISR_FanRPM, FALLING);
	reset();
}

float EvapCooler::getLastTemp() {	
	return lastInTemp;
}

void EvapCooler::setOutTempF(float val) {
	lastOutTemp = val;
}

void EvapCooler::setInTempF(float val) {
	lastInTemp = val;
}

void EvapCooler::setRemoteTempF(float val) {
	lastRemoteTemp = val;
}

void EvapCooler::setThermostatTemp(float val) {
	thermostatTemp = val;
}

void EvapCooler::setAtticTemp(float val) {
	lastAtticTemp = val;
}


//Act like the power just came on...
void EvapCooler::reset() {
	StartTime = millis();
	lastShutOffTime = millis();
	lastOffTime = millis();
	pumpWaiting = false;
	lastCycleTime = millis();
	lastInTemp = 30.0f;
	lastRemoteTemp = 0.0f;
	pinMode(pumpPin, OUTPUT);
	digitalWrite(pumpPin, LOW);
	pinMode(fanPin, OUTPUT);
	digitalWrite(fanPin, LOW);
	pinMode(thermoCoolPin, INPUT);  //Need pull down
	pinMode(thermoFanPin, INPUT);   //Need pull down
	pinMode(rpmPin, INPUT_PULLUP);
}


//For the webpage
bool EvapCooler::getPumpStatus() {
	return (digitalRead(pumpPin) == HIGH);
}

bool EvapCooler::getFanStatus() {
	return (digitalRead(fanPin) == HIGH);
}


//float thermostatTemp = 70.0f;



bool EvapCooler::getThermostatStatus() {
	if (sConfig.VacationMode) {
		if (sunIsUp) return false;
		if (lastRemoteTemp < 65.0) return false;
		if (lastOutTemp < lastRemoteTemp) return true;
	}
	else {
		//Fail safe, logic not working - read the wall thermo
		//if (thermostatTemp < 32.0f) return (digitalRead(thermoCoolPin) == LOW);
		//Bad reading or no reading, use the wall instead
		if (lastRemoteTemp < 32.0f) return (digitalRead(thermoCoolPin) == LOW);

		//We are getting temp updates, so lets finger this out.
		thermostatTemp = sConfig.DayTemp;
		if (!sunIsUp) {
			thermostatTemp = lastOutTemp - 1;
			if (thermostatTemp > sConfig.NightMax) thermostatTemp = sConfig.NightMax;
			if (thermostatTemp < sConfig.NightMin) thermostatTemp = sConfig.NightMin;
		}
		else {
			//Nice evening

		}

		if (lastThermState_ShortCycle) {
			thermostatTemp -= 0.50f;
		}
		else {
			thermostatTemp += 0.10f;
		}

		lastThermState_ShortCycle = (lastRemoteTemp > thermostatTemp);
		return lastThermState_ShortCycle;
	}
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

void EvapCooler::setSunStatus(bool val) {
	sunIsUp = val;	
}

void EvapCooler::statsToJson(void(*printFunction)(const char *)) {
	int vacation = false;
	char buffer[80];
	
	sprintf(buffer, "\"Pump\" : %s,",getPumpStatus() ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"Sun\" : %s,", sunIsUp ? "\"Daytime\"" : "\"Night Time\"");
	printFunction(buffer);

	sprintf(buffer, "\"WaitingForPump\" : %s,", pumpWaiting ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"Fan\" : %s,", getFanStatus() ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"ThermostatCool\" : %s,", getThermostatStatus() ? "true" : "false");
	printFunction(buffer);

	sprintf(buffer, "\"ThermostatSetting\" : %.1f,", thermostatTemp);
	printFunction(buffer);


	sprintf(buffer, "\"LastOutTemp\" : %.1f,", lastOutTemp);
	printFunction(buffer);

	sprintf(buffer, "\"LastTemp\" : %.1f,", lastInTemp);
	printFunction(buffer);

	sprintf(buffer, "\"LastRemoteTemp\" : %.1f,", lastRemoteTemp);
	printFunction(buffer);

	sprintf(buffer, "\"fanRPM\" : %.1f,", fanRPM);
	printFunction(buffer);

	sprintf(buffer, "\"LastAtticTemp\" : %.1f,",  lastAtticTemp);
	printFunction(buffer);

}


void EvapCooler::process() {
	char buffer[80];
	bool fan = false;
	bool pump = false;
	unsigned long now;
	bool curPump = getPumpStatus();
	now = millis();


	//Service the cooler RPM interrupt info
	unsigned long diff = millis() - lastRPMTime;
	//If the diff is over 1000 ms..... (Aka it's not spinning)
	if (diff > 1000) {  
		//Slowly reduce it to zero.......
		fanRPM = fanRPM / 2.0;
	}


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
			if (lastInTemp < 70.0f) {
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
			
			float lowTemp = 70.5f;
			float highTemp = 71.0f;

			//This keeps it running a little colder during the day.
			//After sunset it mellows out, goes for cooling mode.
			if (sunIsUp) {
				lowTemp = 66.0f;
				highTemp = 67.0f;
			}
			//This just give us a gap with no setting in the middle, stops short cycles
			if (lastInTemp < lowTemp) pump = false;
			if (lastInTemp > highTemp) pump = true;

			if (curPump != pump) {
				snprintf(buffer, 80, "Pump Status: %s", pump ? "On" : "Off");
				webLog.println(buffer);
			}
		}
	}
	else {  //ThermoCoolPin == LOW
		if (lastThermState) {
			//Just record the shut off time.......
			lastOffTime = now;
			webLog.It(netTime.getHourFloat(),"Fan Off: T-OFF");
		}
	}

	if (sConfig.VacationMode) pump = false;

	if (sConfig.FanMode == Off) fan = false;
	if (sConfig.PumpMode == Off) pump = false;

	if (sConfig.FanMode == On) fan = true;
	if (sConfig.PumpMode == On) pump = true;

	lastThermState = tempThermState;
	digitalWrite(fanPin, fan);
	digitalWrite(pumpPin, pump);
}

