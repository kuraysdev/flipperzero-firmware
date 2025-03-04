#pragma once

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a.h>

#include <lib/toolbox/simple_array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Iso14443_4aData* iso14443_4a_data;
    FuriString* device_name;
} EmvData;

typedef enum {
    EmvErrorNone,
    EmvErrorNotPresent,
    EmvErrorTimeout,
    EmvErrorProtocol,
} EmvError;

extern const NfcDeviceBase nfc_device_emv;

// Virtual methods

EmvData* emv_alloc(void);

void emv_free(EmvData* data);

void emv_reset(EmvData* data);

void emv_copy(EmvData* data, const EmvData* other);

bool emv_verify(EmvData* data, const FuriString* device_type);

bool emv_load(EmvData* data, FlipperFormat* ff, uint32_t version);

bool emv_save(const EmvData* data, FlipperFormat* ff);

bool emv_is_equal(const EmvData* data, const EmvData* other);

const char* emv_get_device_name(const EmvData* data, NfcDeviceNameType name_type);

const uint8_t* emv_get_uid(const EmvData* data, size_t* uid_len);

bool emv_set_uid(EmvData* data, const uint8_t* uid, size_t uid_len);

Iso14443_4aData* emv_get_base_data(const EmvData* data);

#ifdef __cplusplus
}
#endif
