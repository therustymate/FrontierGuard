#include "common.h"

// Protector Magic Bytes
#define PROTECTOR_EXECUTABLE_HEADER_MAGIC   0x46475045u     // FGPE
#define PROTECTOR_RELOCATION_TABLE_MAGIC    0x50524c54u     // PRLT
#define PROTECTOR_IMPORT_TABLE_MAGIC        0x50494154u     // PIAT

// Protector Status
typedef enum {
    PROTECTOR_SUCCESS                           = 0x100,

    PROTECTOR_ERROR_UNKNOWN                     = 0x200,
    PROTECTOR_ERROR_INVALID_PARAMETER           = 0x210,

    PROTECTOR_RELOCATION_FAILED                 = 0x300,
    PROTECTOR_INVALID_RELOCATION_MAGIC          = 0x310,
    PROTECTOR_DELTA_CALCULATION_FAILED          = 0x320,

    PROTECTOR_IMPORT_FAILED                     = 0x400,
    PROTECTOR_INVALID_IMPORT_MAGIC              = 0x410,
} PROTECTOR_STATUS;

// Protector AES Data
typedef struct {
    DWORD aesId;                // AES Data ID
    uint8_t aes_key[16];        // AES Key
    uint8_t aes_iv[16];         // AES IV
} PROTECTOR_AES_DATA;

// Protector Relocation Table Header
typedef struct {
    DWORD magic;                // PRLT
    DWORD version;              // 1
    DWORD count;                // sizeof(relocation_table)

    DWORD aesId;                // AES Data ID
} PROTECTOR_RELOCATION_HEADER;

// Protector Import Table Header
typedef struct {
    DWORD magic;                // PIAT
    DWORD version;              // 1
    DWORD count;                // sizeof(IAT_table)

    DWORD aesId;                // AES Data ID
} PROTECTOR_IMPORT_HEADER;

// Protector Section Header
typedef struct {
    DWORD magic;                // PSDT
    DWORD version;              // 1
    DWORD size;                 // sizeof(SectionData)

    DWORD aesId;                // AES Data ID
} PROTECTOR_SECTION_HEADER;

// Protector Executable Information Header
typedef struct {
    DWORD           magic;              // FGPE
    DWORD           version;            // 1

    ULONGLONG       imageBase;          // IMAGE_OPTIONAL_HEADER->ImageBase
    DWORD           imageSize;          // IMAGE_OPTIONAL_HEADER->SizeOfImage
    DWORD           headerSize;         // IMAGE_OPTIONAL_HEADER->SizeOfHeaders
    DWORD           entry;              // IMAGE_OPTIONAL_HEADER->AddressOfEntryPoint

    PROTECTOR_RELOCATION_HEADER reloc;  // Protector Relocation Table Header
    PROTECTOR_IMPORT_HEADER import;     // Protector Import Table Header
    PROTECTOR_SECTION_HEADER section;   // Protector Section Header
} PROTECTOR_EXECUTABLE_HEADER;

// Protector Context
typedef struct {
    BYTE*                           base;
    SIZE_T                          size;

    IMAGE_DOS_HEADER*               dos;
    IMAGE_NT_HEADERS*               nt;
    IMAGE_SECTION_HEADER*           sections;

    PROTECTOR_RELOCATION_HEADER*    relocations;

    DWORD                           entry;
} PROTECTOR_CONTEXT, *PPROTECTOR_CONTEXT;

typedef void (*PROTECTOR_ENTRY_POINT)(void);

typedef struct {
    BYTE* address;
    DWORD size;
} SECTION_DATA;