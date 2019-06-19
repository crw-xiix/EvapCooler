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
	enum TriState {
		Off,
		Automatic,
		On
	};
	//For tristate switches 
	const int SettingOff = 0;
	const int SettingAuto = 1;
	const int SettingOn = 2;
private:
	//These times are in milliseconds
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

	//Some overrides.
	TriState fanSetting = Automatic;
	TriState pumpSetting = Automatic;

private: //Settings for times.... note this is in millis
	unsigned long maxDelay = (1000 * 60 * 2);   //2 Minutes
	bool pumpWaiting = false;

	unsigned long cycleIntervalTime = 100;  // 100 ms
	unsigned long lastCycleTime = 0;
private: //Internals for stuff
	bool lastThermState = false;  //Off
	float lastTemp = 30.0f;

public:
	EvapCooler(int iPumpPin, int iFanPin, int iCoolTPin, int iFanTPin);

	float getLastTemp();
	void setTempC(float val);

	void reset();  //Act like the power just came on...

	void overrideFan(TriState val);
	void overridePump(TriState val);

	void statsToJson(void(*printFunction)(const char *));


	bool getPumpStatus();
	bool getFanStatus();

	bool getThermostatStatus();
	bool getThermostatFanOnlyStatus();
	unsigned long getTimeToTurnOn();
	void process();
};