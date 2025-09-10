# MiniUCD CMake configuration file
include(CMakeFindDependencyMacro)

# Create the target
if(NOT TARGET MiniUCD::MiniUCD)
    add_library(MiniUCD::MiniUCD INTERFACE IMPORTED)
    set_target_properties(MiniUCD::MiniUCD PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
    )
endif()

# Set standard package variables
set(MiniUCD_FOUND TRUE)
set(MiniUCD_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include") 
