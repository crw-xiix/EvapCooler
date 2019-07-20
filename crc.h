#pragma once
#ifndef __CRC_H__
#define __CRC_H__ 1

#include <stdint.h>

uint8_t crcSlow(uint8_t const *message, int nBytes);

#endif