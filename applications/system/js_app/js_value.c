#include "js_value.h"
#include <stdarg.h>

size_t js_value_buffer_size(const JsValueDeclaration* declaration) {
    JsValueType type = declaration->type;

    if(type == JsValueTypeString) return 1;

    if(type == JsValueTypeObject) {
        size_t total = 0;
        for(size_t i = 0; i < declaration->n_children; i++) {
            total += js_value_buffer_size(&declaration->children[i]);
        }
        return total;
    }

    return 0;
}

static size_t js_value_resulting_c_values_count(const JsValueDeclaration* declaration) {
    JsValueType type = declaration->type;

    if(type == JsValueTypeObject) {
        size_t total = 0;
        for(size_t i = 0; i < declaration->n_children; i++) {
            total += js_value_resulting_c_values_count(&declaration->children[i]);
        }
        return total;
    }

    return 1;
}

static bool js_value_declaration_valid(const JsValueDeclaration* declaration) {
    JsValueType type = declaration->type;

    // Args can have an arbitrary number of children of arbitrary types
    if(type == JsValueTypeArgs) {
        for(size_t i = 0; i < declaration->n_children; i++)
            if(!js_value_declaration_valid(&declaration->children[i])) return false;
        if(declaration->permit_null) return false;
        return true;
    }

    // Enums can only have EnumValue children
    if(type == JsValueTypeEnum) {
        if(declaration->enum_size != 1 && declaration->enum_size != 2 &&
           declaration->enum_size != 4)
            return false;
        for(size_t i = 0; i < declaration->n_children; i++) {
            const JsValueDeclaration* child = &declaration->children[i];
            if(!js_value_declaration_valid(child) || child->type != JsValueTypeEnumValue)
                return false;
        }
        return true;
    }

    // Objects must have valid children
    if(type == JsValueTypeObject) {
        for(size_t i = 0; i < declaration->n_children; i++) {
            const JsValueDeclaration* child = &declaration->children[i];
            if(!js_value_declaration_valid(child) || !child->object_field_name) return false;
        }
        if(declaration->permit_null) return false;
        return true;
    }

    // EnumValues must have their string field set
    if(type == JsValueTypeEnumValue) {
        return declaration->n_children == 0 && declaration->enum_string_value != NULL;
    }

    // Literal types can't have default values
    if(type == JsValueTypeAny || type == JsValueTypeAnyArray || type == JsValueTypeAnyObject ||
       type == JsValueTypeFunction) {
        if(declaration->permit_null) return false;
    }

    // All other types can't have children
    return declaration->n_children == 0;
}

#define PREPEND_JS_ERROR_AND_RETURN(mjs, flags, ...)                    \
    do {                                                                \
        if((flags) & JsValueParseFlagReturnOnError)                     \
            mjs_prepend_errorf((mjs), MJS_BAD_ARGS_ERROR, __VA_ARGS__); \
        return JsValueParseStatusJsError;                               \
    } while(0)

static void js_value_assign_enum_val(va_list* out_pointers, size_t enum_size, uint32_t value) {
    if(enum_size == 1)
        *va_arg(*out_pointers, uint8_t*) = value;
    else if(enum_size == 2)
        *va_arg(*out_pointers, uint16_t*) = value;
    else if(enum_size == 4)
        *va_arg(*out_pointers, uint32_t*) = value;
}

static bool js_value_is_null_or_undefined(mjs_val_t* val_ptr) {
    return mjs_is_null(*val_ptr) || mjs_is_undefined(*val_ptr);
}

static bool js_value_maybe_assign_default(
    const JsValueDeclaration* declaration,
    mjs_val_t* val_ptr,
    void* destination,
    size_t size) {
    if(declaration->permit_null && js_value_is_null_or_undefined(val_ptr)) {
        memcpy(destination, &declaration->default_value, size);
        return true;
    }
    return false;
}

static JsValueParseStatus js_value_parse_va(
    struct mjs* mjs,
    const JsValueDeclaration* declaration,
    JsValueParseFlag flags,
    mjs_val_t* source,
    mjs_val_t* buffer,
    size_t* buffer_index,
    va_list* out_pointers) {
    // fetch out pointer
    void* destination = NULL;
    if(declaration->type != JsValueTypeEnum && declaration->type != JsValueTypeObject)
        destination = va_arg(*out_pointers, void*);

    switch(declaration->type) {
    // Literal terms
    case JsValueTypeAny: {
        *(mjs_val_t*)destination = *source;
        break;
    }
    case JsValueTypeAnyArray: {
        if(!mjs_is_array(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected array");
        *(mjs_val_t*)destination = *source;
        break;
    }
    case JsValueTypeAnyObject: {
        if(!mjs_is_object(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected object");
        *(mjs_val_t*)destination = *source;
        break;
    }
    case JsValueTypeFunction: {
        if(!mjs_is_function(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected function");
        *(mjs_val_t*)destination = *source;
        break;
    }

    // Primitive types
    case JsValueTypeRawPointer: {
        if(js_value_maybe_assign_default(declaration, source, destination, sizeof(void*))) break;
        if(!mjs_is_foreign(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected pointer");
        *(void**)destination = mjs_get_ptr(mjs, *source);
        break;
    }
    case JsValueTypeInt32: {
        if(js_value_maybe_assign_default(declaration, source, destination, sizeof(int32_t))) break;
        if(!mjs_is_number(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected number");
        *(int32_t*)destination = mjs_get_int32(mjs, *source);
        break;
    }
    case JsValueTypeDouble: {
        if(js_value_maybe_assign_default(declaration, source, destination, sizeof(double))) break;
        if(!mjs_is_number(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected number");
        *(double*)destination = mjs_get_double(mjs, *source);
        break;
    }
    case JsValueTypeBool: {
        if(js_value_maybe_assign_default(declaration, source, destination, sizeof(bool))) break;
        if(!mjs_is_boolean(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected bool");
        *(bool*)destination = mjs_get_bool(mjs, *source);
        break;
    }
    case JsValueTypeString: {
        if(js_value_maybe_assign_default(declaration, source, destination, sizeof(const char*)))
            break;
        if(!mjs_is_string(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected string");
        buffer[*buffer_index] = *source;
        *(const char**)destination = mjs_get_string(mjs, &buffer[*buffer_index], NULL);
        (*buffer_index)++;
        break;
    }

    // Types with children
    case JsValueTypeArgs: {
        furi_check(source == JS_VAL_PARSE_SOURCE_ARGS);
        size_t args_provided = mjs_nargs(mjs);
        for(size_t i = 0; i < declaration->n_children; i++) {
            mjs_val_t arg = (i < args_provided) ? mjs_arg(mjs, i) : MJS_UNDEFINED;
            JsValueParseStatus status = js_value_parse_va(
                mjs, declaration, flags, &arg, buffer, buffer_index, out_pointers);
            if(status != JsValueParseStatusOk)
                PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "argument %zu: ", i);
        }
        break;
    }
    case JsValueTypeEnum: {
        if(declaration->permit_null && js_value_is_null_or_undefined(source)) {
            js_value_assign_enum_val(
                out_pointers, declaration->enum_size, declaration->default_value.enum_val);
        } else if(mjs_is_string(*source)) {
            const char* str = mjs_get_string(mjs, source, NULL);
            furi_check(str);
            bool match_found = false;
            for(size_t i = 0; i < declaration->n_children; i++) {
                const JsValueDeclaration* child = &declaration->children[i];
                furi_check(child->type == JsValueTypeEnumValue);
                if(strcmp(str, child->enum_string_value) == 0) {
                    js_value_assign_enum_val(
                        out_pointers, declaration->enum_size, child->enum_value);
                    match_found = true;
                    break;
                }
            }
            if(!match_found)
                PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected one of permitted strings");
        } else {
            PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected string");
        }
        break;
    }

    case JsValueTypeObject: {
        if(!mjs_is_object(*source)) PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "expected object");
        for(size_t i = 0; i < declaration->n_children; i++) {
            const JsValueDeclaration* child = &declaration->children[i];
            mjs_val_t field = mjs_get(mjs, *source, child->object_field_name, ~0);
            JsValueParseStatus status =
                js_value_parse_va(mjs, child, flags, &field, buffer, buffer_index, out_pointers);
            if(status != JsValueParseStatusOk)
                PREPEND_JS_ERROR_AND_RETURN(mjs, flags, "field %s: ", child->object_field_name);
        }
        break;
    }

    case JsValueTypeEnumValue:
        furi_crash();
    }

    return JsValueParseStatusOk;
}

JsValueParseStatus js_value_parse(
    struct mjs* mjs,
    const JsValueDeclaration* declaration,
    JsValueParseFlag flags,
    mjs_val_t* buffer,
    size_t buf_size,
    mjs_val_t* source,
    size_t n_c_vals,
    ...) {
    furi_check(mjs);
    furi_check(declaration);
    furi_check(buffer);

#ifdef JS_VAL_DEBUG
    furi_check(js_value_declaration_valid(declaration));
    furi_check(buf_size == js_value_buffer_size(declaration));
    furi_check(n_c_vals == js_value_resulting_c_values_count(declaration));
#else
    UNUSED(js_value_declaration_valid);
    UNUSED(js_value_resulting_c_values_count);
#endif

    va_list out_pointers;
    va_start(out_pointers, n_c_vals);

    size_t buffer_index = 0;
    JsValueParseStatus status =
        js_value_parse_va(mjs, declaration, flags, source, buffer, &buffer_index, &out_pointers);
    furi_check(buffer_index <= buf_size);

    va_end(out_pointers);

    return status;
}
