#pragma once

#include <nfc/protocols/emv/emv.h>

#include "../nfc_protocol_support_render_common.h"

void nfc_render_emv_info(const EmvData* data, NfcProtocolFormatType format_type, FuriString* str);

void nfc_render_emv_data(const EmvData* data, FuriString* str);
