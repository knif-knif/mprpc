#aux_source_directory(. SRC_LIST)
set(SRC_LIST rpcapp.cc rpcconfig.cc rpcheader.pb.cc rpcprovider.cc rpcchannel.cc rpccontroller.cc logger.cc zookeeperutil.cc)

add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)