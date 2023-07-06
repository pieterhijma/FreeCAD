
add_subdirectory(App)

set(Projection_Scripts
    Init.py
)

if(BUILD_GUI)
    list (APPEND Projection_Scripts InitGui.py)
endif(BUILD_GUI)

add_custom_target(ProjectionScripts ALL
    SOURCES ${Projection_Scripts}
)

fc_target_copy_resource(ProjectionScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Projection
    ${Projection_Scripts})

INSTALL(
    FILES
        ${Projection_Scripts}
    DESTINATION
        Mod/Projection
)

