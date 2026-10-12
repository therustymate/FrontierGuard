#pragma once

#include "Guard.h"
#include <windows.h>
#include <stdint.h>
#include <time.h>

#define HBG_MAGIC_CONTEXT "HBGCTX"
#define HBG_TAIL_CANARY UINT32_C(0xDEADBEEF)

typedef enum _HB_ERROR {
    HB_SUCCESS,
    HB_FAILED,
    HB_ERROR_UNKNOWN,
    HB_ERROR_INVALID_PARAMETERS,
    HB_ERROR_MAX_REACHED,
    HB_ERROR_GUARD_ALREADY_EXISTS,
    HB_COMPROMISED,
} HB_ERROR;

typedef enum _HB_INTEGRITY_FLAGS {
    HB_INTEGRITY_OK             = 0,
    HB_PRIMARY_HASH_INVALID     = 1 << 0,
    HB_SHADOW_HASH_INVALID      = 1 << 1,
    HB_CONFIG_MISMATCH          = 1 << 2,
    HB_CONTEXT_ID_MISMATCH      = 1 << 3,
    HB_SHADOW_ID_MISMATCH       = 1 << 4,
    HB_CONTEXT_MAGIC_INVALID    = 1 << 5,
    HB_TAIL_CANARY_INVALID      = 1 << 6,
} HB_INTEGRITY_FLAGS;

typedef enum _HEARTBEAT_STATUS {
    HEARTBEAT_SUCCESS,
    HEARTBEAT_FAILED,
    HEARTBEAT_COMPROMISED,
} HEARTBEAT_STATUS;

typedef struct _HeartbeatGuardConfig {
    int                             GuardId;
    int                             CheckOnSuccess;
    int                             CheckOnFail;
    int                             CheckOnError;
    uint64_t                        HeartbeatTimeoutMs;
    uint64_t                        ExecutionTimeoutMs;
} HeartbeatGuardConfig;

typedef struct _HeartbeatGuardContext {
    // Identification
    unsigned char                   Magic[sizeof(HBG_MAGIC_CONTEXT)-1];
    int                             GuardId;

    // Configuration
    HeartbeatGuardConfig            Config;

    // Runtime
    GUARD_ERROR                     Result;
    HEARTBEAT_STATUS                Status;
    uint64_t                        LastHeartbeatTimeMs;
    uint64_t                        LastExecutionDurationMs;

    // Integrity
    unsigned char                   ConfigHash[32];
    uint32_t                        TailCanary;
} HeartbeatGuardContext;

typedef struct _HeartbeatGuardShadowConfig {
    HeartbeatGuardConfig            Config;
    SHA256                          ConfigHash;
} HeartbeatGuardShadowConfig;

typedef struct _HeartbeatGuardEntry {
    HeartbeatGuardContext* Context;
    const HeartbeatGuardShadowConfig* Shadow;
} HeartbeatGuardEntry;