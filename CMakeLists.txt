cmake_minimum_required(VERSION 4.1)
project(fopenbangs_backend C)

# Settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Executable
include_directories(include)
add_executable(${PROJECT_NAME} src/main.c
        src/registry.c
        src/mongoose/mongoose.c)

# Public directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/public"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/public"
        COMMENT "Moving public/ assets to the build directory..."
)