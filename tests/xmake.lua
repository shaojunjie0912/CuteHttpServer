target("test_http_context", function()
    set_kind("binary")
    add_files("http/test_http_context.cpp")
    add_deps("cutehttpserver")
end)
