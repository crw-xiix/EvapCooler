/*
#include "ScheduleObject.h"

ScheduleObject* ScheduleObject::Get(const char *str) {
	//Format is:  function,vars, vars,vars,vars, no commas in vars
	int func = 0;
	int args = sscanf(str, "%d:", &func);
	//Meh, returning on the stack, ick
	if (func == 0) return NULL;

	const char *ptr = strchr(str, ':');
	if (!ptr) return NULL;
	ptr++;
	//ourTasks is not null, there is something in it.....
	for (int i = 0;; i++) {
		if (ourTasks[i].func == NULL) break;		//we hit the end of the array
		if (ourTasks[i].Id == func) return ourTasks[i].func(ptr);
	}
	return NULL;
}

void *ScheduleObject::operator new(size_t size) {
	return ramBase.allocate(size);
}

FixedRam<ScheduleObject> ScheduleObject::ramBase = FixedRam<ScheduleObject>("Schedule", 256);
*/