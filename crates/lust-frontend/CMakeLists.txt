option(LUST_BUILD_FRONTEND_SHARED "" ON)

set(LUST_FRONTEND_SOURCES
    private/lexer.cpp
    private/misc.cpp
    private/grammar.cpp
)

if (LUST_BUILD_FRONTEND_SHARED)
    add_library(LustFrontend SHARED
        ${LUST_FRONTEND_SOURCES}
    )
else()
    add_library(LustFrontend STATIC
        ${LUST_FRONTEND_SOURCES}
    )
endif()

add_library(Lust::Frontend ALIAS LustFrontend)

target_include_directories(LustFrontend 
    PUBLIC
        public
    PRIVATE
        private
)

target_set_api_macro(LustFrontend)
target_precompile_headers(LustFrontend PUBLIC
    public/public_pch.hpp
)
