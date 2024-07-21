add_rules("mode.debug", "mode.release")

target("socket_server")
    set_kind("binary")
    add_files("src/socket/server.c")
    add_files("src/socket/socket.c")

target("socket_client")
    set_kind("binary")
    add_files("src/socket/client.c")
    add_files("src/socket/socket.c")

target("socket_benchmark")
    set_kind("binary")
    add_files("src/socket/benchmark.c")
    add_files("src/socket/socket.c")

