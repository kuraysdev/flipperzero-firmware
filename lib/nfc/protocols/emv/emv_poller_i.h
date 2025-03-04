#pragma once

#include "emv_poller.h"

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EmvPollerStateIdle,
    EmvPollerStateSelectPPSE,
    EmvPollerStateReadFailed,
    EmvPollerStateReadSuccess,

    EmvPollerStateNum,
} EmvPollerState;

struct EmvPoller {
    Iso14443_4aPoller* iso14443_4a_poller;
    EmvPollerState state;
    EmvError error;
    EmvData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    BitBuffer* input_buffer;
    BitBuffer* result_buffer;
    EmvPollerEventData emv_event_data;
    EmvPollerEvent emv_event;
    NfcGenericEvent general_event;
    NfcGenericCallback callback;
    void* context;
};

EmvError emv_process_error(Iso14443_4aError error);

EmvError emv_process_status_code(uint8_t status_code);

EmvError emv_poller_select_ppse(EmvPoller* instance, EmvData* data);

const EmvData* emv_poller_get_data(EmvPoller* instance);

#ifdef __cplusplus
}
#endif
