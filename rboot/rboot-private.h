#ifndef __RBOOT_PRIVATE_H__
#define __RBOOT_PRIVATE_H__

//////////////////////////////////////////////////
// rBoot open source boot loader for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//////////////////////////////////////////////////

typedef int int32;
typedef unsigned int uint32;
typedef unsigned char uint8;

#include "rboot.h"

#define NOINLINE __attribute__ ((noinline))

#define ROM_MAGIC	   0xe9
#define ROM_MAGIC_NEW1 0xea
#define ROM_MAGIC_NEW2 0x04

// buffer size, must be at least 0x10 (size of rom_header_new structure)
#define BUFFER_SIZE 0x100

// esp8266 built in rom functions
extern uint32 SPIRead(uint32 addr, void *outptr, uint32 len);
extern uint32 SPIEraseSector(int);
extern uint32 SPIWrite(uint32 addr, void *inptr, uint32 len);
extern void ets_printf(char*, ...);
extern void ets_delay_us(int);
void *ets_memset(void *s, int c, size_t n);
void *ets_memcpy(void *dest, const void *src, size_t n);

// functions we'll call by address
typedef void stage2a(uint32);
typedef void usercode(void);

// standard rom header
typedef struct {
	// general rom header
	uint8 magic;
	uint8 count;
	uint8 flags1;
	uint8 flags2;
	usercode* entry;
} rom_header;

typedef struct {
	uint8* address;
	uint32 length;
} section_header;

// new rom header (irom section first) there is
// another 8 byte header straight afterward the
// standard header
typedef struct {
	// general rom header
	uint8 magic;
	uint8 count; // second magic for new header
	uint8 flags1;
	uint8 flags2;
	uint32 entry;
	// new type rom, lib header
	uint32 add; // zero
	uint32 len; // length of irom section
} rom_header_new;

#ifdef MAPPED_FLASH

#ifndef FLASH_MAP
#define FLASH_MAP(c) ((uint32 *)(0x40200000 + (c)))
#endif

// http://richard.burtons.org/2015/06/12/esp8266-cache_read_enable/
void Cache_Read_Enable(uint8 odd_even, uint8 mb_count, uint8 no_idea);
// http://esp8266-re.foogod.com/wiki/Cache_Read_Disable
void Cache_Read_Disable(void);

#define SPIRead SPImemcpy
static uint32 SPImemcpy(uint32 addr, void *outptr, uint32 len) {
	if (len & 0x03) {
		ets_printf(__FILE__ ": %d: addr=%p ptr=%p len=%d\r\n", __LINE__, addr, outptr, len);
		return 1;
	}
	ets_memcpy(outptr, FLASH_MAP(addr), len);
	return 0;
}

#endif // MAPPED_FLASH

#endif