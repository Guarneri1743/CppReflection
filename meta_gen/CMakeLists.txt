add_clang_executable(meta_gen
    meta_gen.cpp
)

target_link_libraries(meta_gen
    PRIVATE
    clangTooling
    clangBasic
    clangASTMatchers
)