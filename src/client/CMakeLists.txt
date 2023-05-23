#包含该目录下所有的文件
aux_source_directory(. SRC_LIST)

#指定生成可执行的文件
add_executable(ChatClient ${SRC_LIST})
#指定可执行文件连接需要依赖的文件
target_link_libraries(ChatClient pthread)