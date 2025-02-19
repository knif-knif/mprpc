#include "rpcchannel.h"
#include "rpcheader.pb.h"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "rpcapp.h"
#include "rpccontroller.h"

/*
header_size service_name method_name args_size args
*/

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                google::protobuf::Message* response, google::protobuf::Closure* done) 
{
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    int args_size = 0;
    std::string args_str;

    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    }
    else {
        controller->SetFailed("serialize request error!");
        return;
    }
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    std::string rpc_header_str;
    uint32_t header_size = 0;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    }
    else {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0, reinterpret_cast<char*>(&header_size), sizeof(header_size));
    send_rpc_str += rpc_header_str + args_str;

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd) {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    std::string ip = RpcApp::GetInstance().GetConfig().Load("RpcServerIp");
    uint16_t port = atoi(RpcApp::GetInstance().GetConfig().Load("RpcServerPort").c_str());
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) {
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
        return;
    }
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }
    //std::string response_str(recv_buf, 0, recv_size);
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        char errtxt[2048] = {0};
        sprintf(errtxt, "parse error! recv_buf:%s", recv_buf);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    close(clientfd);
}