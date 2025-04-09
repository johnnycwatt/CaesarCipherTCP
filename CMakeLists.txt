cmake_minimum_required(VERSION 3.10)
project(CaesarCipher)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific: Link Winsock library
if (WIN32)
    link_libraries(ws2_32)
endif()

add_library(common STATIC
        common/caesar_cipher.cpp
)
target_include_directories(common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/common
)

# Server executable
add_executable(server
        server/server.cpp
)
target_link_libraries(server PRIVATE common)

# Client executable
add_executable(client
        client/client.cpp
)
target_link_libraries(client PRIVATE common)

# Output executables to bin/
set_target_properties(server client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)