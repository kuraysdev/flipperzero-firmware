#include "../lfrfid_i.h"

void lfrfid_scene_t5577_password_input_on_enter(void* context) {
    LfRfid* app = context;
    ByteInput* byte_input = app->byte_input;
    if(app->t5577_password_task == LfRfidT5577PasswordTaskSet) {
        byte_input_set_header_text(byte_input, "Set new password");
        byte_input_set_result_callback(
            byte_input,
            lfrfid_text_input_callback,
            NULL,
            app,
            app->t5577_new_password,
            T5577_PASSWORD_SIZE);
    } else if(app->t5577_password_task == LfRfidT5577PasswordTaskWrite) {
        if(app->current_password_set == false) {
            byte_input_set_header_text(byte_input, "Enter current password");
            byte_input_set_result_callback(
                byte_input,
                lfrfid_text_input_callback,
                NULL,
                app,
                app->t5577_current_password,
                T5577_PASSWORD_SIZE);
        } else {
            byte_input_set_header_text(byte_input, "Enter new password");
            byte_input_set_result_callback(
                byte_input,
                lfrfid_text_input_callback,
                NULL,
                app,
                app->t5577_new_password,
                T5577_PASSWORD_SIZE);
        }
    } else if(app->t5577_password_task == LfRfidT5577PasswordTaskRemove) {
        byte_input_set_header_text(byte_input, "Enter current password");
        byte_input_set_result_callback(
            byte_input,
            lfrfid_text_input_callback,
            NULL,
            app,
            app->t5577_current_password,
            T5577_PASSWORD_SIZE);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewByteInput);
}

bool lfrfid_scene_t5577_password_input_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    uint32_t current_password;
    uint32_t new_password;
    memcpy(&current_password, app->t5577_current_password, T5577_PASSWORD_SIZE);
    memcpy(&new_password, app->t5577_new_password, T5577_PASSWORD_SIZE);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == LfRfidEventNext) {
            consumed = true;
            if(app->t5577_password_task == LfRfidT5577PasswordTaskSet) {
                lfrfid_worker_set_new_t5577_password(app->lfworker, new_password);
                scene_manager_next_scene(scene_manager, LfRfidSceneWrite);
            } else if(app->t5577_password_task == LfRfidT5577PasswordTaskWrite) {
                if(app->current_password_set == false) {
                    app->current_password_set = true;
                    lfrfid_worker_set_current_t5577_password(app->lfworker, current_password);
                    scene_manager_next_scene(scene_manager, LfRfidSceneT5577PasswordInput);
                } else {
                    lfrfid_worker_set_new_t5577_password(app->lfworker, new_password);
                    scene_manager_next_scene(scene_manager, LfRfidSceneWrite);
                }
            } else if(app->t5577_password_task == LfRfidT5577PasswordTaskRemove) {
                lfrfid_worker_set_current_t5577_password(app->lfworker, current_password);
                scene_manager_next_scene(scene_manager, LfRfidSceneWrite);
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(scene_manager, LfRfidSceneSaveData, 0);
        size_t size = protocol_dict_get_data_size(app->dict, app->protocol_id);
        protocol_dict_set_data(app->dict, app->protocol_id, app->old_key_data, size);
    }

    return consumed;
}

void lfrfid_scene_t5577_password_input_on_exit(void* context) {
    UNUSED(context);
}
