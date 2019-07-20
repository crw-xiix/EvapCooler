#include "Blocker.h"
#include <algorithm>


int CompareBlockData(BlockData left, BlockData right) {
	return (left.when > right.when);
}

Blocker::Blocker() {
	blockList = std::vector<BlockData>();
	blockList.reserve(100);
}

int Blocker::blockIndexOf(IPAddress val) {
	int i;
	for (i = 0; i < blockList.size(); i++) {
		if (val == blockList[i].IP) {
			blockList[i].IP;
			return i;
		}
	}
	return -1;
}

bool Blocker::IsBlocked(IPAddress val, float when) {
	int v = blockIndexOf(val);
	if (v >= 0) {
		if (blockList[v].when > when) {
			return true;
		}
		else {
			//Remove them.
			blockList.erase(blockList.begin() + v);
		}
	}
	return false;
}
void Blocker::Clear() {
	blockList.clear();
}

void Blocker::BlockIP(IPAddress val, float when) {

	BlockData temp;
	temp.IP = val;
	temp.when = when + 0.10f;
	if (blockList.size() >= (max)) {
		sort(blockList.begin(), blockList.end(), CompareBlockData);
		blockList.erase(blockList.end() - 1);
	}
	if (blockList.size() < (max)) blockList.push_back(temp);

};