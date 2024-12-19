#ifndef __CRC_DEFINES__
#define __CRC_DEFINES__
//
//    FILE: CrcDefines.h
//  AUTHOR: vovagorodok
// PURPOSE: Arduino class for CRC
//     URL: https://github.com/RobTillaart/CRC


#include <Arduino.h>


#if defined(CRC_CUSTOM_SIZE)
using crc_size_t = CRC_CUSTOM_SIZE;
#elif defined(__AVR__)
using crc_size_t = uint16_t;
#else
using crc_size_t = uint32_t;
#endif

#endif

