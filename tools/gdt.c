#include "memory.h"
#include "stdint.h"

typedef struct
{

	uint16_t Lower_Limit;
	uint16_t Lower_Base;

	uint8_t Middle_Base;

	// Access Byte
	uint8_t Accessed : 1;
	uint8_t Readable_Writable : 1;
	uint8_t Direction_Conforming : 1;
	uint8_t Executable : 1;
	uint8_t Type : 1;
	uint8_t Privilege : 2;
	uint8_t Present : 1;

	uint8_t Limit : 4;

	// flags
	uint8_t _Reserved : 1;
	uint8_t Long : 1;
	uint8_t Size : 1;
	uint8_t Granularity : 1;

	uint8_t Upper_Base;
} __attribute__((__packed__)) GDT_Entry;

typedef struct
{
	uint16_t size;
	uint32_t offset;
} __attribute__((__packed__)) GDT_Descriptor;

GDT_Entry GDT_Table[3];
GDT_Descriptor GDT_Desc;

void gdt_start()
{
	memset(&GDT_Table[0], 0, sizeof(GDT_Entry));
	// kernel code segment
	GDT_Table[1] = (GDT_Entry){
	    .Lower_Limit = 0xFFFF,
	    .Lower_Base = 0x0000,
	    .Middle_Base = 0x00,
	    .Accessed = 1,
	    .Readable_Writable = 1,
	    .Direction_Conforming = 0,
	    .Executable = 1,
	    .Type = 1,
	    .Privilege = 0,
	    .Present = 1,
	    ._Reserved = 0,
	    .Long = 0,
	    .Size = 1,
	    .Granularity = 1,
	};
	// kernel data segment
	GDT_Table[2] = (GDT_Entry){
	    .Lower_Limit = 0xFFFF,
	    .Lower_Base = 0x0000,
	    .Middle_Base = 0x00,
	    .Accessed = 1,
	    .Readable_Writable = 1,
	    .Direction_Conforming = 0,
	    .Executable = 0,
	    .Type = 1,
	    .Privilege = 0,
	    .Present = 1,
	    ._Reserved = 0,
	    .Long = 0,
	    .Size = 1,
	    .Granularity = 1,
	};

	/* user code segment
	GDT_Table[3] = (GDT_Entry){
	    .Lower_Limit = 0xFFFF,
	    .Lower_Base = 0x0000,
	    .Middle_Base = 0x00,
	    .Accessed = 1,
	    .Readable_Writable = 1,
	    .Direction_Conforming = 0,
	    .Executable = 1,
	    .Type = 1,
	    .Privilege = 3,
	    .Present = 1,
	    ._Reserved = 0,
	    .Long = 0,
	    .Size = 1,
	    .Granularity = 1,
	};
	*/
	/* user data segment
	GDT_Table[4] = (GDT_Entry){
	    .Lower_Limit = 0xFFFF,
	    .Lower_Base = 0x0000,
	    .Middle_Base = 0x00,
	    .Accessed = 1,
	    .Readable_Writable = 1,
	    .Direction_Conforming = 0,
	    .Executable = 0,
	    .Type = 1,
	    .Privilege = 3,
	    .Present = 1,
	    ._Reserved = 0,
	    .Long = 0,
	    .Size = 1,
	    .Granularity = 1,
	};
	*/

	GDT_Desc.offset = GDT_Table;
	GDT_Desc.size = sizeof(GDT_Table) - 1;

	asm(".intel_syntax noprefix");
	asm("cli"
	    "lgdt[_GDT_Desc];"
	    "mov eax, cr0;"
	    "or al, 1;"
	    "mov cr0, eax;");
}