#pragma once

class WebEntry {
public:
	float timeOfDay;
	char message[80];
	void Clear() {
		message[0] = 0;
		timeOfDay = 0;
	}
};

class WebLog {
private:
	int size = 20;
	int headPtr = 0;
	int tailPtr = 0;
	WebEntry *entries;
public:

	WebLog(int isize);
	int FreeRam();
	void It(float time, const char *message);
	void println(const char *msg);
	void Print(void(*printFunction)(const char *));
	void PrintReverse(void(*printFunction)(const char *));
	void Clear();

};

extern WebLog webLog;
