add_executable(SOIS_Template "")

target_sources(SOIS_Template 
PRIVATE
    main.cpp
)

target_include_directories(SOIS_Template 
PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}
    ${DependenciesDirectory}/glm)

target_link_libraries(SOIS_Template 
PRIVATE
    SOIS
)

target_compile_definitions(SOIS_Template PRIVATE GLFW_INCLUDE_NONE)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(SOIS_Template PRIVATE opengl32.lib)

    target_compile_options(SOIS_Template PRIVATE 
    PUBLIC
        -permissive- -std:c++17
    PRIVATE
        -WX- -W4
    )
endif()

set_target_properties(SOIS_Template PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${AssetDirectory}/Bin)
