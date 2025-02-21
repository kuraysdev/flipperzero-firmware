#include "../nfc_app_i.h"

const char* const debug_text[] = {
    "OFF",
    "ON",
};

enum NfcDebugIndex {
    NfcDebugIndexField,
    NfcDebugIndexParserDebug,
};

static void nfc_scene_debug_var_list_enter_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static void nfc_scene_debug_parser_debug_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    NfcApp* nfc = variable_item_get_context(item);

    variable_item_set_current_value_text(item, debug_text[index]);
    if(index) {
        FURI_LOG_D("NFC", "NFC parser debug enabled");
        nfc->debug_settings->parser_debug = true;
    } else {
        FURI_LOG_D("NFC", "NFC parser debug disabled");
        nfc->debug_settings->parser_debug = false;
    }

    if(nfc_settings_save(nfc->debug_settings)) {
        FURI_LOG_D("NFC", "Debug settings saved");
    } else {
        FURI_LOG_E("NFC", "Failed to save debug settings");
    }
}

void nfc_scene_debug_on_enter(void* context) {
    NfcApp* nfc = context;
    VariableItem* item;

    variable_item_list_reset(nfc->var_item_list);

    variable_item_list_add(nfc->var_item_list, "Field", 1, NULL, NULL);

    item = variable_item_list_add(
        nfc->var_item_list,
        "Parser debug",
        COUNT_OF(debug_text),
        nfc_scene_debug_parser_debug_changed,
        nfc);
    variable_item_set_current_value_index(item, nfc->debug_settings->parser_debug);
    variable_item_set_current_value_text(item, debug_text[nfc->debug_settings->parser_debug]);

    // TODO: Add logging toggle here when #4039 finished

    variable_item_list_set_enter_callback(
        nfc->var_item_list, nfc_scene_debug_var_list_enter_callback, nfc);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewVariableItemList);
}

bool nfc_scene_debug_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcDebugIndexField) {
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneDebug, NfcDebugIndexField);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneField);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_debug_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
