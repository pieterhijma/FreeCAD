if(MSVC)
    add_definitions(-DFCAppDrawing -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H -DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
endif(MSVC)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${XercesC_INCLUDE_DIRS}
)
link_directories(${OCC_LIBRARY_DIR})

set(Projection_LIBS
    Part
    Spreadsheet
    FreeCADApp
)

SET(Projection_SRCS
    AppProjection.cpp
    AppProjectionPy.cpp
    PreCompiled.cpp
    PreCompiled.h
)

SET(ProjectionAlgos_SRCS
    DrawingExport.cpp
    DrawingExport.h
    ProjectionAlgos.cpp
    ProjectionAlgos.h
)

SOURCE_GROUP("Mod" FILES ${Projection_SRCS})
SOURCE_GROUP("Algorithms" FILES ${ProjectionAlgos_SRCS})

SET(ProjectionComplete_SRCS
    ${Projection_SRCS}
    ${ProjectionAlgos_SRCS}
)

if(FREECAD_USE_PCH)
    add_definitions(-D_PreComp_)
    GET_MSVC_PRECOMPILED_SOURCE("PreCompiled.cpp" PCH_SRCS ${ProjectionComplete_SRCS})
    ADD_MSVC_PRECOMPILED_HEADER(Projection PreCompiled.h PreCompiled.cpp PCH_SRCS)
endif(FREECAD_USE_PCH)

add_library(Projection SHARED ${Projection_SRCS} ${ProjectionAlgos_SRCS})
target_link_libraries(Projection ${Projection_LIBS})

SET_BIN_DIR(Projection Projection /Mod/Projection)
SET_PYTHON_PREFIX_SUFFIX(Projection)

INSTALL(TARGETS Projection DESTINATION ${CMAKE_INSTALL_LIBDIR})
