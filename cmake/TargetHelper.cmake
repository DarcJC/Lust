
function(target_set_api_macro target_name)
    string(TOUPPER ${target_name} TARGET_NAME_UPPER)
    string(TOLOWER ${target_name} TARGET_NAME_LOWER)
    generate_export_header(${target_name}
            EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_API"
            NO_EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_INTERNAL"
            EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/export_headers/${TARGET_NAME_LOWER}_export.h"
    )
    target_compile_definitions(${target_name} PRIVATE
            "${target_name}_EXPORTS"
            "${TARGET_NAME_UPPER}_EXPORTS"
    )
endfunction()
