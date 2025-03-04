#include "emv_i.h"

#include <furi.h>

#define EMV_PROTOCOL_NAME "EMV"

const NfcDeviceBase nfc_device_emv = {
    .protocol_name = EMV_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)emv_alloc,
    .free = (NfcDeviceFree)emv_free,
    .reset = (NfcDeviceReset)emv_reset,
    .copy = (NfcDeviceCopy)emv_copy,
    .verify = (NfcDeviceVerify)emv_verify,
    .load = (NfcDeviceLoad)emv_load,
    .save = (NfcDeviceSave)emv_save,
    .is_equal = (NfcDeviceEqual)emv_is_equal,
    .get_name = (NfcDeviceGetName)emv_get_device_name,
    .get_uid = (NfcDeviceGetUid)emv_get_uid,
    .set_uid = (NfcDeviceSetUid)emv_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)emv_get_base_data,
};

EmvData* emv_alloc(void) {
    EmvData* data = malloc(sizeof(EmvData));
    data->iso14443_4a_data = iso14443_4a_alloc();
    data->device_name = furi_string_alloc();
    return data;
}

void emv_free(EmvData* data) {
    furi_check(data);

    emv_reset(data);
    iso14443_4a_free(data->iso14443_4a_data);
    furi_string_free(data->device_name);
    free(data);
}

void emv_reset(EmvData* data) {
    furi_check(data);

    iso14443_4a_reset(data->iso14443_4a_data);
}

void emv_copy(EmvData* data, const EmvData* other) {
    furi_check(data);
    furi_check(other);

    emv_reset(data);

    iso14443_4a_copy(data->iso14443_4a_data, other->iso14443_4a_data);
}

bool emv_verify(EmvData* data, const FuriString* device_type) {
    UNUSED(data);
    return furi_string_equal_str(device_type, EMV_PROTOCOL_NAME);
}

bool emv_load(EmvData* data, FlipperFormat* ff, uint32_t version) {
    furi_check(data);
    furi_check(ff);

    return iso14443_4a_load(data->iso14443_4a_data, ff, version);
}

bool emv_save(const EmvData* data, FlipperFormat* ff) {
    furi_check(data);
    furi_check(ff);

    return iso14443_4a_save(data->iso14443_4a_data, ff);
}

bool emv_is_equal(const EmvData* data, const EmvData* other) {
    furi_check(data);
    furi_check(other);

    return iso14443_4a_is_equal(data->iso14443_4a_data, other->iso14443_4a_data);
}

const char* emv_get_device_name(const EmvData* data, NfcDeviceNameType name_type) {
    furi_check(data);

    if(name_type == NfcDeviceNameTypeFull) {
        furi_string_printf(
            data->device_name,
            "EMV Card\n\e#%s",
            iso14443_4a_get_device_name(data->iso14443_4a_data, name_type));
        return furi_string_get_cstr(data->device_name);
    }

    return iso14443_4a_get_device_name(data->iso14443_4a_data, name_type);
}

const uint8_t* emv_get_uid(const EmvData* data, size_t* uid_len) {
    furi_check(data);
    furi_check(uid_len);

    return iso14443_4a_get_uid(data->iso14443_4a_data, uid_len);
}

bool emv_set_uid(EmvData* data, const uint8_t* uid, size_t uid_len) {
    furi_check(data);

    return iso14443_4a_set_uid(data->iso14443_4a_data, uid, uid_len);
}

Iso14443_4aData* emv_get_base_data(const EmvData* data) {
    furi_check(data);

    return data->iso14443_4a_data;
}
