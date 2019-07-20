#pragma once


struct TempLogEntry {

	uint8_t Pump : 1;
	uint8_t TempIF : 7;
	uint8_t Fan : 1;
	uint8_t TempOF : 7;
	uint8_t Water : 8;

	TempLogEntry(float tempI, float tempO, bool pump, bool fan, float water) {
		TempIF = (int)tempI;
		TempOF = (int)tempO;
		Fan = fan;
		Pump = pump;
		Water = (int)water; //Limit to 255 gallons
	}
	TempLogEntry() {
		Pump = false;
		TempIF = 68;
		TempOF = 68;
		Fan = false;
		Water = 0;
	}
	/*
	TempLogEntry(const TempLogEntry &tle) {
	Pump = tle.Pump;
	Fan = tle.Fan;
	TempOF = tle.TempOF;
	TempIF = tle.TempIF;
	}*/
};

class TempLog {
	static const int entries = 24; //24*12 - 5 minutes
	TempLogEntry log[entries];
public:
	void Set(float time, TempLogEntry data) {
		int idx = (int)time;
		if ((idx < 0) || (idx >= entries)) idx = 0;
		log[idx] = data;
	}

	void Set(float time, float tempI, float tempO, bool pump, bool fan, float water) {
		//
		Set(time, TempLogEntry(tempI, tempO, pump, fan, water));
	}

	void Clear() {
		for (int i = 0; i < entries; i++) {
			log[i] = TempLogEntry();
		};

	}
	TempLog() {
		Clear();
	}

	void OutputLog(void(*pf)(const char*)) {
		for (int i = 0; i < entries; i++) {
			char buffer[80];
			snprintf(buffer, 80, "ITemp:%d OTemp:%d Fan:%s Pump:%s Water: %d/Gal",
				log[i].TempIF,
				log[i].TempOF,
				log[i].Fan ? "On" : "Off",
				log[i].Pump ? "On" : "Off",
				log[i].Water
			);
			pf(buffer);
		}
	}
};

