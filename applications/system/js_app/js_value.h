#pragma once

#include <furi.h>
#include "js_modules.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    JsValueTypeArgs, //<! Function arguments
    JsValueTypeAny, //<! Literal term
    JsValueTypeAnyArray, //<! Literal term, after ensuring that it's an array
    JsValueTypeAnyObject, //<! Literal term, after ensuring that it's an object
    JsValueTypeFunction, //<! Literal term, after ensuring that it's a function
    JsValueTypeRawPointer, //<! Unchecked `void*`
    JsValueTypeInt32, //<! Number cast to `int32_t`
    JsValueTypeDouble, //<! Number cast to `double`
    JsValueTypeString, //<! Any string cast to `const char*`
    JsValueTypeBool, //<! Bool cast to `bool`
    JsValueTypeEnum, //<! String with predefined possible values cast to the desired C enum
    JsValueTypeObject, //<! Object with predefined recursive fields cast to several C values

    JsValueTypeEnumValue, //<! String-to-number enum mapping
} JsValueType;

typedef union {
    void* ptr_val;
    int32_t int32_val;
    double double_val;
    const char* str_val;
    uint32_t enum_val;
    bool bool_val;
} JsValueDefaultValue;

typedef struct JsValueDeclaration {
    JsValueType type;
    union {
        const char* object_field_name;
        const char* enum_string_value;
    };
    bool permit_null;
    JsValueDefaultValue default_value;
    union {
        uint8_t enum_size;
        uint32_t enum_value;
    };
    size_t n_children;
    const struct JsValueDeclaration* children;
} JsValueDeclaration;

#define JS_VALUE_ENUM_VARIANT(str_value, int_value) \
    {.type = JsValueTypeEnumValue, .enum_string_value = str_value, .enum_value = int_value}

#define JS_VALUE_CHILDREN(variants_or_fields) \
    .n_children = COUNT_OF(variants_or_fields), .children = variants_or_fields

typedef enum {
    JsValueParseFlagNone = 0,
    JsValueParseFlagReturnOnError =
        (1
         << 0), //<! Sets mjs error string to a description of the parsing error and returns from the JS function
} JsValueParseFlag;

typedef enum {
    JsValueParseStatusOk, //<! Parsing completed successfully
    JsValueParseStatusJsError, //<! Parsing failed due to incorrect JS input
} JsValueParseStatus;

/**
 * @brief Magic value that tells `js_value_parse` to source the JS values from
 * function arguments.
 */
#define JS_VAL_PARSE_SOURCE_ARGS NULL

/**
 * @brief Determines the size of the buffer array of `mjs_val_t`s that needs to
 * be passed to `js_value_parse`.
 */
size_t js_value_buffer_size(const JsValueDeclaration* declaration);

/**
 * @brief Converts a JS value into a series of C values.
 * 
 * @param[in]    mjs         mJS instance pointer
 * @param[in]    declaration Type declaration for the input value
 * @param[in]    flags       See the corresponding enum
 * @param[out]   buffer      Temporary buffer for values that need to live
 *                           longer than the function call. To determine the
 *                           size of the buffer, use `js_value_buffer_size`.
 *                           Values parsed by this function will become invalid
 *                           when this buffer goes out of scope.
 * @param[in]    buf_size    Number of entries in the temporary buffer (i.e.
 *                           `COUNT_OF`, not `sizeof`).
 * @param[in]    source      Source JS value that needs to be converted. Set to
 *                           `JS_VAL_PARSE_SOURCE_ARGS` to fetch the values from
 *                           arguments.
 * @param[in]    n_c_vals    Number of output C values
 * @param[out]   ...         Pointers to output C values. The order in which
 *                           these values are populated corresponds to the order
 *                           in which the values are defined in the declaration.
 * 
 * @returns Parsing status
 */
JsValueParseStatus js_value_parse(
    struct mjs* mjs,
    const JsValueDeclaration* declaration,
    JsValueParseFlag flags,
    mjs_val_t* buffer,
    size_t buf_size,
    mjs_val_t* source,
    size_t n_c_vals,
    ...);

#define JS_VALUE_PARSE(mjs, declaration, flags, status_ptr, value_ptr, ...) \
    void* _args[] = {__VA_ARGS__};                                          \
    size_t _n_args = COUNT_OF(_args);                                       \
    size_t _temp_buf_len = js_value_buffer_size(declaration);               \
    mjs_val_t _temp_buffer[_temp_buf_len];                                  \
    *(status_ptr) = js_value_parse(                                         \
        mjs, declaration, flags, _temp_buffer, _temp_buf_len, value_ptr, _n_args, __VA_ARGS__);

#define JS_VALUE_PARSE_ARGS_OR_RETURN(mjs, declaration, ...) \
    JsValueParseStatus _status;                              \
    JS_VALUE_PARSE(                                          \
        mjs,                                                 \
        declaration,                                         \
        JsValueParseFlagReturnOnError,                       \
        &_status,                                            \
        JS_VAL_PARSE_SOURCE_ARGS,                            \
        __VA_ARGS__);                                        \
    if(_status != JsValueParseStatusOk) return;

#ifdef __cplusplus
}
#endif
