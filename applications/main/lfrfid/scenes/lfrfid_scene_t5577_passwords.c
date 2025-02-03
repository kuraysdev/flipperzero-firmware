#include "../lfrfid_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    SubmenuIndexSetNewPassword,
    SubmenuIndexWriteWithPassword,
    SubmenuIndexDisablePassword,
} SubmenuIndex;

void lfrfid_scene_t5577_passwords_submenu_callback(void* context, uint32_t index) {
    LfRfid* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void lfrfid_scene_t5577_passwords_on_enter(void* context) {
    LfRfid* app = context;
    Submenu* submenu = app->submenu;

    lfrfid_worker_set_current_t5577_password(app->lfworker, 0);
    app->t5577_current_password = 0;
    lfrfid_worker_set_new_t5577_password(app->lfworker, 0);
    app->t5577_new_password = 0;
    lfrfid_worker_set_write_mode(app->lfworker, LFRFIDWorkerWriteModeDefault);
    app->current_password_set = false;

    submenu_add_item(
        submenu,
        "Set Password",
        SubmenuIndexSetNewPassword,
        lfrfid_scene_t5577_passwords_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Write with Password",
        SubmenuIndexWriteWithPassword,
        lfrfid_scene_t5577_passwords_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Disable Password",
        SubmenuIndexDisablePassword,
        lfrfid_scene_t5577_passwords_submenu_callback,
        app);
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, LfRfidSceneT5577Passwords));

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewSubmenu);
}

bool lfrfid_scene_t5577_passwords_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSetNewPassword) {
            app->t5577_password_task = LfRfidT5577PasswordTaskSet;
            lfrfid_worker_set_write_mode(app->lfworker, LFRFIDWorkerWriteModeT5577SetPassword);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectKey);
            consumed = true;
        } else if(event.event == SubmenuIndexDisablePassword) {
            app->t5577_password_task = LfRfidT5577PasswordTaskRemove;
            lfrfid_worker_set_write_mode(app->lfworker, LFRFIDWorkerWriteModeT5577RemovePassword);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteWithPassword) {
            app->t5577_password_task = LfRfidT5577PasswordTaskWrite;
            lfrfid_worker_set_write_mode(app->lfworker, LFRFIDWorkerWriteModeT5577WithPassword);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectKey);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, LfRfidSceneT5577Passwords, event.event);

    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(app->scene_manager, LfRfidSceneT5577Passwords, 0);
    }

    return consumed;
}

void lfrfid_scene_t5577_passwords_on_exit(void* context) {
    LfRfid* app = context;

    submenu_reset(app->submenu);
}
