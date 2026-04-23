# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	lmp91000-potentiostat
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_lmp91000-potentiostat target,
# which contains the content of the SLC project
include(lmp91000-potentiostat.cmake)

add_executable(lmp91000-potentiostat
    # Add additional sources here
)

target_include_directories(lmp91000-potentiostat PUBLIC
    # Add additional include paths here
)

target_compile_definitions(lmp91000-potentiostat PUBLIC
    # Add additional macros here
)

target_compile_options(lmp91000-potentiostat PUBLIC
    # Set additional compiler flags here
)

target_link_options(lmp91000-potentiostat PUBLIC
    # Set additional linker flags here
    -u _printf_float
)

# Link with the content defined in the SLC project
target_link_libraries(lmp91000-potentiostat PRIVATE
    slc
)

# Include managed project content if available
include(lmp91000-potentiostat_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(lmp91000-potentiostat PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET lmp91000-potentiostat
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:lmp91000-potentiostat>" "$<TARGET_FILE_DIR:lmp91000-potentiostat>/$<TARGET_FILE_BASE_NAME:lmp91000-potentiostat>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:lmp91000-potentiostat>" "$<TARGET_FILE_DIR:lmp91000-potentiostat>/$<TARGET_FILE_BASE_NAME:lmp91000-potentiostat>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:lmp91000-potentiostat>" "$<TARGET_FILE_DIR:lmp91000-potentiostat>/$<TARGET_FILE_BASE_NAME:lmp91000-potentiostat>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET lmp91000-potentiostat
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()