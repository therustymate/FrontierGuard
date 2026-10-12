#include "EventGuard.h"
#include "Crypto.h"

static HeartbeatGuardEntry      ctx_storage[MAX_GUARDS];
static size_t                   ctx_storage_size;

static HB_ERROR validate_entry(HeartbeatGuardEntry* entry, unsigned int* issues) {
    if (entry == NULL || issues == NULL) {
        return HB_ERROR_INVALID_PARAMETERS;
    }

    *issues = HB_INTEGRITY_OK;

    if (entry->Context == NULL || entry->Shadow == NULL) {
        return HB_FAILED;
    }

    const HeartbeatGuardContext*        ctx     = entry->Context;
    const HeartbeatGuardShadowConfig*   shadow  = entry->Shadow;

    if (memcmp(ctx->Magic, HBG_MAGIC_CONTEXT, sizeof(HBG_MAGIC_CONTEXT) - 1) != 0) {
        *issues |= HB_CONTEXT_MAGIC_INVALID;
    }

    if (ctx->TailCanary != HBG_TAIL_CANARY) {
        *issues |= HB_TAIL_CANARY_INVALID;
    }

    SHA256 ctx_hash;
    SHA256 shadow_hash;

    if (!sha256(&ctx->Config, sizeof(ctx->Config), ctx_hash))           { return HB_FAILED; }
    if (!sha256(&shadow->Config, sizeof(shadow->Config), shadow_hash))  { return HB_FAILED; }

    if (memcmp(ctx_hash, ctx->ConfigHash, sizeof(SHA256)) != 0) {
        *issues |= HB_PRIMARY_HASH_INVALID;
    }

    if (memcmp(shadow_hash, shadow->ConfigHash, sizeof(SHA256)) != 0) {
        *issues |= HB_SHADOW_HASH_INVALID;
    }

    if (memcmp(ctx_hash, shadow_hash, sizeof(SHA256)) != 0) {
        *issues |= HB_CONFIG_MISMATCH;
    }

    if (ctx->GuardId != ctx->Config.GuardId) {
        *issues |= HB_CONTEXT_ID_MISMATCH;
    }

    if (ctx->Config.GuardId != shadow->Config.GuardId) {
        *issues |= HB_SHADOW_ID_MISMATCH;
    }

    if (*issues != HB_INTEGRITY_OK) {
        return HB_COMPROMISED;
    }

    return HB_SUCCESS;
}

static HB_ERROR create_shadow(const HeartbeatGuardShadowConfig* source, HeartbeatGuardShadowConfig** output
) {
    if (output == NULL) {
        return HB_ERROR_INVALID_PARAMETERS;
    }

    *output = NULL;

    if (source == NULL) {
        return HB_ERROR_INVALID_PARAMETERS;
    }

    HeartbeatGuardShadowConfig* shadow = VirtualAlloc(
        NULL,
        sizeof(HeartbeatGuardShadowConfig),
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (shadow == NULL) {
        return HB_FAILED;
    }

    *shadow = *source;

    DWORD oldProtect;

    if (!VirtualProtect(shadow, sizeof(*shadow), PAGE_READONLY, &oldProtect)) {
        VirtualFree(shadow, 0, MEM_RELEASE);
        return HB_FAILED;
    }

    *output = shadow;
    return HB_SUCCESS;
}

HB_ERROR register_guard(HeartbeatGuardContext* ctx) {
    if (ctx == NULL) {
        return HB_ERROR_INVALID_PARAMETERS;
    }

    if (ctx_storage_size >= MAX_GUARDS) {
        return HB_ERROR_MAX_REACHED;
    }

    if (ctx->GuardId <= 0 || ctx->GuardId != ctx->Config.GuardId) {
        return HB_ERROR_INVALID_PARAMETERS;
    }

    if (memcmp(ctx->Magic, HBG_MAGIC_CONTEXT, sizeof(HBG_MAGIC_CONTEXT) - 1) != 0 || ctx->TailCanary != HBG_TAIL_CANARY) {
        return HB_COMPROMISED;
    }

    for (size_t i=0; i < ctx_storage_size; i++) {
        const HeartbeatGuardEntry* entry = &ctx_storage[i];
        if (
            entry->Context->GuardId         == ctx->GuardId ||
            entry->Shadow->Config.GuardId   == ctx->GuardId ||
            entry->Context->Config.GuardId  == ctx->GuardId
        ) {
            return HB_ERROR_GUARD_ALREADY_EXISTS;
        }
    }

    HeartbeatGuardShadowConfig source = {0};
    memcpy(&source.Config, &ctx->Config, sizeof(HeartbeatGuardConfig));

    SHA256 hash;
    if (!sha256(&source.Config, sizeof(source.Config), hash)) {
        return HB_FAILED;
    }
    memcpy(source.ConfigHash, hash, sizeof(SHA256));

    HeartbeatGuardShadowConfig* shadow = NULL;

    HB_ERROR result = create_shadow(&source, &shadow);

    if (result != HB_SUCCESS) {
        return result;
    }

    memcpy(ctx->ConfigHash, hash, sizeof(SHA256));

    HeartbeatGuardEntry* entry = &ctx_storage[ctx_storage_size];

    entry->Context = ctx;
    entry->Shadow = shadow;

    ctx_storage_size++;

    return HB_SUCCESS;
}