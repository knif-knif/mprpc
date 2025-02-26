#include "rpcprovider.h"
#include "rpcapp.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"
#include "logger.h"

void RpcProvider::NotifyService(google::protobuf::Service *service) {
    ServiceInfo service_info;

    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    std::string service_name = pserviceDesc->name();
    int methodCnt = pserviceDesc->method_count();

    LOG_INFO("service_name: %s", service_name.c_str());
    //std::cout << "service_name:" << service_name << std::endl;

    for (int i = 0; i < methodCnt; ++i) {
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});

        LOG_INFO("method_name: %s", method_name.c_str());
        // std::cout << "method_name:" << method_name << std::endl;
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run() {
    std::string ip = RpcApp::GetInstance().GetConfig().Load("RpcServerIp");
    uint16_t port = atoi(RpcApp::GetInstance().GetConfig().Load("RpcServerPort").c_str());
    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
    std::placeholders::_2, std::placeholders::_3));
    server.setThreadNum(4);

    ZkClient zkCli;
    zkCli.Start();
    for (auto &sp : m_serviceMap) {
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap) {
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "RpcProvider start service at " << ip << ":" << port << std::endl;
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {

}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    std::string recv_buf = buffer->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);
    
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else {
        std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
    }
    std::string args_str = recv_buf.substr(4 + header_size, args_size);
    std::cout << "========================\n"
    << "header_size:" << header_size << "\n" << "rpc_header_str:" << rpc_header_str << "\n" 
    << "service_name:" << service_name << "\t" << "method_name:" << method_name << "\t" << "args_size:" << args_size << "\t" << "args_str:" << args_str << std::endl 
    << "======================\n"; 

    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end()) {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) {
        std::cout << service_name << " => " << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request error:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);
    service->CallMethod(method, nullptr, request, response, done);
}   

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    if (response->SerializeToString(&response_str)) {
        conn->send(response_str);
    }
    else {
        std::cout << "Serialize response error" << std::endl;
    }
    conn->shutdown();
}