
add_library(LustDefines INTERFACE)
add_library(Lust::Defines ALIAS LustDefines)

target_compile_definitions(LustDefines INTERFACE
    LUST_VERSION="0.0.1"
)
