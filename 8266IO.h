#pragma once

//Same as the stock one but is better about errors, see code file
bool readBytesUntil(WiFiClient& client, char* dest, char terminator, int len, unsigned long timeout_ms);
