option(LUST_BUILD_AST_VISUALIZER "Enable lust-ast-visualizer target" ON)

if (LUST_BUILD_AST_VISUALIZER)

    check_include_file("graphviz/gvc.h" FOUND_GRAPHVIZ)

    if (FOUND_GRAPHVIZ)
        set(VISUALIZER_SOURCES
            src/main.cpp
        )

        add_executable(LustASTVisualizer ${VISUALIZER_SOURCES})
        add_executable(Lust::ASTVisualizer ALIAS LustASTVisualizer)

        target_link_libraries(LustASTVisualizer PRIVATE gvc cgraph cxxopts)
        target_link_libraries(LustASTVisualizer PRIVATE Lust::Defines Lust::Frontend)
    else() 
        message(WARNING "Can't locate graphviz")
    endif()

endif()
