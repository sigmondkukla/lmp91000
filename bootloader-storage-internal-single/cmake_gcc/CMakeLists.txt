# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	bootloader-storage-internal-single
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_bootloader-storage-internal-single target,
# which contains the content of the SLC project
include(bootloader-storage-internal-single.cmake)

add_executable(bootloader-storage-internal-single
    # Add additional sources here
)

target_include_directories(bootloader-storage-internal-single PUBLIC
    # Add additional include paths here
)

target_compile_definitions(bootloader-storage-internal-single PUBLIC
    # Add additional macros here
)

target_compile_options(bootloader-storage-internal-single PUBLIC
    # Set additional compiler flags here
)

target_link_options(bootloader-storage-internal-single PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(bootloader-storage-internal-single PRIVATE
    slc
)

# Include managed project content if available
include(bootloader-storage-internal-single_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(bootloader-storage-internal-single PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET bootloader-storage-internal-single
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:bootloader-storage-internal-single>" "$<TARGET_FILE_DIR:bootloader-storage-internal-single>/$<TARGET_FILE_BASE_NAME:bootloader-storage-internal-single>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:bootloader-storage-internal-single>" "$<TARGET_FILE_DIR:bootloader-storage-internal-single>/$<TARGET_FILE_BASE_NAME:bootloader-storage-internal-single>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:bootloader-storage-internal-single>" "$<TARGET_FILE_DIR:bootloader-storage-internal-single>/$<TARGET_FILE_BASE_NAME:bootloader-storage-internal-single>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET bootloader-storage-internal-single
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()