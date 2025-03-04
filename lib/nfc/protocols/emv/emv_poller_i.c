#include "emv_poller_i.h"

#include <furi.h>

#include "emv_i.h"

#define TAG "EMVPoller"

EmvError emv_process_error(Iso14443_4aError error) {
    switch(error) {
    case Iso14443_4aErrorNone:
        return EmvErrorNone;
    case Iso14443_4aErrorNotPresent:
        return EmvErrorNotPresent;
    case Iso14443_4aErrorTimeout:
        return EmvErrorTimeout;
    default:
        return EmvErrorProtocol;
    }
}

EmvError emv_poller_select_ppse(EmvPoller* instance, EmvData* data) {
    furi_check(instance);
    furi_check(data);
    const uint8_t emv_select_ppse_cmd[] = {
        0x00, 0xA4, // SELECT ppse
        0x04, 0x00, // P1:By name, P2: empty
        0x0e, // Lc: Data length
        0x32, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, // Data string:
        0x53, 0x2e, 0x44, 0x44, 0x46, 0x30, 0x31, // 2PAY.SYS.DDF01 (PPSE)
        0x00 // Le
    };

    FURI_LOG_D(TAG, "Select PPSE");

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_bytes(
        instance->input_buffer, emv_select_ppse_cmd, sizeof(emv_select_ppse_cmd));

    EmvError error = EmvErrorNone;

    do {
        iso14443_4a_poller_send_block(
            instance->iso14443_4a_poller, instance->input_buffer, instance->result_buffer);

        if(!emv_decode_response(
               bit_buffer_get_data(instance->result_buffer),
               bit_buffer_get_size_bytes(instance->result_buffer))) {
            error = EmvErrorProtocol;
        }
    } while(false);

    return error;
}
