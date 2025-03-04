#pragma once

#include "emv.h"

#define EMV_TAG_APP_TEMPLATE    0x61
#define EMV_TAG_AID             0x4F
#define EMV_TAG_PRIORITY        0x87
#define EMV_TAG_PDOL            0x9F38
#define EMV_TAG_CARD_NAME       0x50
#define EMV_TAG_FCI             0xBF0C
#define EMV_TAG_LOG_CTRL        0x9F4D
#define EMV_TAG_TRACK_1_EQUIV   0x56
#define EMV_TAG_TRACK_2_EQUIV   0x57
#define EMV_TAG_PAN             0x5A
#define EMV_TAG_AFL             0x94
#define EMV_TAG_EXP_DATE        0x5F24
#define EMV_TAG_COUNTRY_CODE    0x5F28
#define EMV_TAG_CURRENCY_CODE   0x9F42
#define EMV_TAG_CARDHOLDER_NAME 0x5F20

bool emv_decode_response(const uint8_t* buff, uint16_t len);
