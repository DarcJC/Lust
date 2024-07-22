
function(add_single_file_test_target name)
    set(target_name lust-tests-${name})
    add_executable(${target_name}
        "${name}.cpp"
    )
    target_include_directories(${target_name} PUBLIC
        headers
    )
    target_compile_definitions(${target_name} PRIVATE
        LUST_TEST_NAME="${name}"
    )
    target_link_libraries(${target_name} PRIVATE
        Lust::Frontend
    )
endfunction(add_single_file_test_target)

add_single_file_test_target(lexer-simple)
add_single_file_test_target(parser-simple)
add_single_file_test_target(simple-string)
