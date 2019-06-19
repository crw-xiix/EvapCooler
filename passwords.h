#pragma once

//The name of the device
const char *wifiHostName = "Chicken";
//Name of the wifi network to connect to 
const char *wifiNetwork = "Wood"; 
//Password for the Wifi network
const char *wifiPassword = "slipperywhendusty";

//USE COMMA, NOT . FOR THIS.
//IE:  192,168,0,1

#define DEVICE_IP 192,168,89,101
#define DEVICE_GATEWAY 192,168,89,1
#define DEVICE_SUBNET 255,255,255,0
#define DEVICE_DNS1 8,8,8,8
#define DEVICE_DNS2 8,8,4,4

// Charles's place
#define DEVICE_LAT 37.676
#define DEVICE_LON -118.087

//Jordan's place
/*
#define DEVICE_LAT 33.481837
#define DEVICE_LON -97.070883
*/

//Uncomment the one that is correct for you........

#define DEVICE_TZ -8  //Pacific standard time
//#define DEVICE_TZ -7  //Mountain standard time/pacific daylight
//#define DEVICE_TZ -6  //Central standard time
//#define DEVICE_TZ -5  //Eastern standard time



