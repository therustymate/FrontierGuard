#include "common.h"

typedef enum {
    HOOK_ABSOLUTE_JUMP,
    HOOK_RELATIVE_JUMP,
} HOOK_TYPE;

typedef struct {
    PVOID OriginalFunction;
    PVOID HookFunction;
    HOOK_TYPE HookType;
} HOOK_CONTEXT, *PHOOK_CONTEXT;