#pragma once
/*
This runs the evaporative cooler in the house.....
*/

/*
We have some states to deal with here.

	Normal funtion, 1-5 minute delay on fan after water.

	Water Low - Pump is off.




*/


class EvapCooler {
public:
	
	//For tristate switches 
	const int SettingOff = 0;
	const int SettingAuto = 1;
	const int SettingOn = 2;
private:
	//These times are in milliseconds
	bool sunIsUp = true;
	unsigned long StartTime;
	//Last time we turned off the water pump;
	unsigned long lastShutOffTime;
	//When the switch turned on
	unsigned long lastOffTime;

	//pin assignments (in/out)
	int pumpPin;
	int fanPin;
	int thermoCoolPin;
	int thermoFanPin;
	int rpmPin;
	//Some overrides.
	//Mode switches are now in config............

private: //Settings for times.... note this is in millis
	unsigned long maxDelay = (1000 * 60 * 2);   //2 Minutes
	bool pumpWaiting = false;

	unsigned long cycleIntervalTime = 100;  // 100 ms
	unsigned long lastCycleTime = 0;
private: //Internals for stuff
	bool lastThermState = false;  //Off
	bool lastThermState_ShortCycle = false;
	//These are reading from gauges
	float lastInTemp = 30.0f;
	float lastOutTemp = 30.0f;
	float lastRemoteTemp = 30.0f;
	float lastAtticTemp = 30.0f;
	//This is the setpoint for operation, default off (0)
	float thermostatTemp = 0.0f;
	/*public class {
		float operator() { return 
	};*/

public:
	EvapCooler(int iPumpPin, int iFanPin, int iCoolTPin, int iFanTPin, int iRPMPin);

	float getLastTemp();
	void setInTempF(float val);
	void setOutTempF(float val);
	void setRemoteTempF(float val);
	//Less than 32F means use the wall thermostat
	void setThermostatTemp(float val);
	void setAtticTemp(float val);
	void reset();  //Act like the power just came on...


	void statsToJson(void(*printFunction)(const char *));

	void setSunStatus(bool val);

	bool getPumpStatus();
	bool getFanStatus();

	bool getThermostatStatus();
	bool getThermostatFanOnlyStatus();
	unsigned long getTimeToTurnOn();
	void process();
};