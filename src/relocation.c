#include "common.h"
#include "protector.h"

static PROTECTOR_STATUS applyRelocation(PROTECTOR_CONTEXT* ctx) {


    return PROTECTOR_RELOCATION_FAILED;
}

static PROTECTOR_STATUS calculateDelta(PROTECTOR_CONTEXT* ctx, ULONG_PTR* out) {
    if (ctx == NULL || out == NULL) {
        return PROTECTOR_ERROR_INVALID_PARAMETER;
    }
    *out = (ULONG_PTR)ctx->base - (ULONG_PTR)ctx->nt->OptionalHeader.ImageBase;
    return PROTECTOR_SUCCESS;
}

PROTECTOR_STATUS relocate(PROTECTOR_CONTEXT* ctx) {
    ULONG_PTR delta;
    PROTECTOR_STATUS status = calculateDelta(ctx, &delta);
    if (status != PROTECTOR_SUCCESS) {
        return status;
    }

    if (delta == 0) {
        // No relocation is required.
        return PROTECTOR_SUCCESS;
    }

    

    return PROTECTOR_SUCCESS;
}