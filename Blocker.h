#pragma once
#ifndef __Blocker_H__
#define __Blocker_H__ 1

#include <ESP8266WiFi.h>
#include <vector>

struct BlockData {
	IPAddress IP;
	float when;  //secs past midnight.....
	int attempts;
};

class Blocker {
	std::vector<BlockData> blockList;
	const int max = 20;
public:
	Blocker();
	void BlockIP(IPAddress val, float when);
	//0-Max, -1 
	int blockIndexOf(IPAddress val);
	bool IsBlocked(IPAddress val, float when);
	void Clear();

};


#endif



