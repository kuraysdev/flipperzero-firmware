#include "emv_i.h"

#define TAG "EMV"

bool emv_decode_response(const uint8_t* buff, uint16_t len) {
    uint16_t i = 0;
    uint16_t tag = 0, first_byte = 0;
    uint16_t tlen = 0;
    bool success = false;

    while(i < len) {
        first_byte = buff[i];
        if((first_byte & 31) == 31) { // 2-byte tag
            tag = buff[i] << 8 | buff[i + 1];
            i++;
            FURI_LOG_T(TAG, " 2-byte TLV EMV tag: %x", tag);
        } else {
            tag = buff[i];
            FURI_LOG_T(TAG, " 1-byte TLV EMV tag: %x", tag);
        }
        i++;
        tlen = buff[i];
        if((tlen & 128) == 128) { // long length value
            i++;
            tlen = buff[i];
            FURI_LOG_T(TAG, " 2-byte TLV length: %d", tlen);
        } else {
            FURI_LOG_T(TAG, " 1-byte TLV length: %d", tlen);
        }
        i++;
        if((first_byte & 32) == 32) { // "Constructed" -- contains more TLV data to parse
            FURI_LOG_T(TAG, "Constructed TLV %x", tag);
            if(!emv_decode_response(&buff[i], tlen)) {
                FURI_LOG_T(TAG, "Failed to decode response for %x", tag);
                // return false;
            } else {
                success = true;
            }
        } else {
            switch(tag) {
            case EMV_TAG_AID:
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_AID %x", tag);
                break;
            case EMV_TAG_PRIORITY:
                success = true;
                break;
            case EMV_TAG_CARD_NAME:
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_CARD_NAME %x", tag);
                break;
            case EMV_TAG_PDOL:
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_PDOL %x (len=%d)", tag, tlen);
                break;
            case EMV_TAG_AFL:
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_AFL %x (len=%d)", tag, tlen);
                break;
            case EMV_TAG_TRACK_1_EQUIV: {
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_TRACK_1_EQUIV %x", tag);
                break;
            }
            case EMV_TAG_TRACK_2_EQUIV: {
                success = true;
                FURI_LOG_T(TAG, "found EMV_TAG_TRACK_2_EQUIV %x", tag);
                break;
            }
            case EMV_TAG_PAN:
                success = true;
                break;
            case EMV_TAG_EXP_DATE:
                success = true;
                break;
            case EMV_TAG_CURRENCY_CODE:
                success = true;
                break;
            case EMV_TAG_COUNTRY_CODE:
                success = true;
                break;
            }
        }
        i += tlen;
    }
    return success;
}
