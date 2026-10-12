#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <string.h>

#include "Crypto.h"

int sha256(const void* data, size_t size, SHA256* output) {
    BCRYPT_ALG_HANDLE algorithm = NULL;
    unsigned char digest[sizeof(SHA256)];
    static const unsigned char empty = 0;
    NTSTATUS status;

    if (output == NULL || (data == NULL && size != 0) || size > UINT32_MAX) {
        return 0;
    }

    status = BCryptOpenAlgorithmProvider(
        &algorithm,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        0
    );

    if (!BCRYPT_SUCCESS(status)) {
        return 0;
    }

    status = BCryptHash(
        algorithm,
        NULL,
        0,
        (PUCHAR)(data != NULL ? data : &empty),
        (ULONG)size,
        digest,
        sizeof(SHA256)
    );

    BCryptCloseAlgorithmProvider(algorithm, 0);

    if (!BCRYPT_SUCCESS(status)) {
        return 0;
    }

    memcpy(output, digest, sizeof(SHA256));

    return 1;
}