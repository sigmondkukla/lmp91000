# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	potentiostat
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_potentiostat target,
# which contains the content of the SLC project
include(potentiostat.cmake)

add_executable(potentiostat
    # Add additional sources here
)

target_include_directories(potentiostat PUBLIC
    # Add additional include paths here
)

target_compile_definitions(potentiostat PUBLIC
    # Add additional macros here
)

target_compile_options(potentiostat PUBLIC
    # Set additional compiler flags here
)

target_link_options(potentiostat PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(potentiostat PRIVATE
    slc
)

# Include managed project content if available
include(potentiostat_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(potentiostat PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET potentiostat
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:potentiostat>" "$<TARGET_FILE_DIR:potentiostat>/$<TARGET_FILE_BASE_NAME:potentiostat>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:potentiostat>" "$<TARGET_FILE_DIR:potentiostat>/$<TARGET_FILE_BASE_NAME:potentiostat>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:potentiostat>" "$<TARGET_FILE_DIR:potentiostat>/$<TARGET_FILE_BASE_NAME:potentiostat>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET potentiostat
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()