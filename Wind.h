#pragma once


class WindSensor {
private:
	static int pin;
	static volatile float lastHz;
	static volatile int state;
	static volatile int reps;
	static volatile unsigned long last_start;
	static volatile bool first;
	static volatile int bounce;
	static float multiplier;

	static float calcHz();

	//User adjustable values.................

	//How many reeds switches per rev?
	static const float clicksPerRev;

	//How many us (micros) in a second?
	static const float microsInSecond;
	
public:
	// Multiplier is for simple wind calc, real one will be fancier
	static void Init(int pin, float multiplier);
	static float GetValue();
	static void ISRRoutine();
};
