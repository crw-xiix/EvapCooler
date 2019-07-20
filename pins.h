#pragma once

/*
GP5  D1 I2C SCL
GP4  D2 I2C SDA
GP0  D3 Water Pump - Out
GP2  D4 Wind - In(Will also flash on board LED)
GP14 D5 Thermo - In(Cooling)
GP12 D6 Water Level - Out
GP13 D7 Water Level - In
GP15 D8 Cooler Fan - Out
? ? ? ? A0 Wind direction(has to be analog)
*/

//

//We only have 9 pins for this......
#define PIN_I2CSCL SCL			//GP5 D1  I2C
#define PIN_I2CSDA SDA			//GP4 D2  I2C

#define PIN_THERMOSTATC D7		//GP14 D7 Thermostat - In
#define PIN_WATER_IN	D0			//GP14 D0 Water Level - In
#define PIN_WATER_OUT	D5		//GP12 D5 Water Level - Out
#define PIN_RPM_IN		D6			//Hall effect sensor on RPMs
#define PIN_FAN_OUT		D8			//GP15 D8 Cooler Fan - Out 
#define PIN_PUMP_OUT	D3			//GP0  D3 Water Pump - Out


#define DEBUG true
