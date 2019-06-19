#include "Wind.h"
#include <ESP8266WiFi.h>




//7 of these
int WindSensor::pin;
volatile float WindSensor::lastHz;
volatile int WindSensor::state;
volatile int WindSensor::reps;
volatile unsigned long WindSensor::last_start;
volatile bool WindSensor::first;
volatile int WindSensor::bounce;

float WindSensor::multiplier;

//How many reeds switches per rev?
const float WindSensor::clicksPerRev = 2.0f;

//How many us (micros) in a second?
const float WindSensor::microsInSecond = 1000000.0f;


void WindSensor::Init(int ipin, float imultiplier) {
	//7 of these
	pin = ipin;
	lastHz = 0;
	state = 0;
	reps = 0;
	last_start = 0;
	first = true;
	bounce = 0;
	multiplier = imultiplier;
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pin), ISRRoutine, CHANGE);
}

float WindSensor::GetValue() {
	//Just in case it's too slow, zero it.
	if ((calcHz()*multiplier) < 1.0f) {
		//Reset us, since we will need a couple loops to get going again.
		//Don't want a partial spin up to give a burst of speed (1mph = 100, etc)
		first = true;
		return 0.0f;
	}
	return lastHz*multiplier;
}

float WindSensor::calcHz() {
	unsigned long diff = (unsigned long)micros() - last_start;
	return (1.0f / clicksPerRev) / (diff / microsInSecond);
}

void WindSensor::ISRRoutine() {
	//To help see it........

	state = digitalRead(pin);
	//We want it going low, we are grounding the interrupt pin
	if (state == 1) return;
	//Toss out value, save time, bail 
	if (first) {
		last_start = micros();
		first = false;
		return;
	}
	unsigned long cur_time = micros();
	unsigned long diff = (unsigned long)cur_time - last_start;
	//This will debounce it
	// 0.000500 seconds
	if (diff < 500) {
		bounce++;
		return;
	}
	//How many reed switch trips...
	reps++;

	//averaging it over two pulses
	float hz = (lastHz + calcHz()) / 2.0;
	lastHz = hz;
	last_start = cur_time;
}

