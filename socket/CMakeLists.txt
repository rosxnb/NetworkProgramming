add_library(${SOCKET_TARGET} OBJECT
    src/socket.cpp
)

target_include_directories(${SOCKET_TARGET} PUBLIC
    include
)
