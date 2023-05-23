#包含该目录下所有的文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./database DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)

#指定生成可执行的文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})
#指定可执行文件连接需要依赖的文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)
