add_executable(tcp_time_server
    tcp_time_server.cpp
)
target_link_libraries(tcp_time_server PRIVATE socket)
