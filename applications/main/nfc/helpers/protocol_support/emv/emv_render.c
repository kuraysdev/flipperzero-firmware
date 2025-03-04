#include "emv_render.h"

#include "../iso14443_4a/iso14443_4a_render.h"

void nfc_render_emv_info(const EmvData* data, NfcProtocolFormatType format_type, FuriString* str) {
    nfc_render_iso14443_4a_brief(emv_get_base_data(data), str);

    if(format_type != NfcProtocolFormatTypeFull) return;

    furi_string_cat(str, "\n\e#ISO14443-4 data");
    nfc_render_iso14443_4a_extra(emv_get_base_data(data), str);
}

void nfc_render_emv_data(const EmvData* data, FuriString* str) {
    nfc_render_emv_info(data, NfcProtocolFormatTypeFull, str);
}
