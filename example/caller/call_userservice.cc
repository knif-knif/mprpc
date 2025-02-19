#include <iostream>
#include "rpcapp.h"
#include "user.pb.h"
#include "rpcchannel.h"

int main(int argc, char **argv) {
    RpcApp::Init(argc, argv);
    fixbug::UserServiceRpc_Stub stub(new RpcChannel());
    fixbug::LoginRequest request;
    request.set_name("kk");
    request.set_pwd("123");
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);
    if (response.result().errcode() == 0) {
        std::cout << "rpc login response:" << response.success() << std::endl;
    }
    else {
        std::cout << "rpc login response error:" << response.result().errmsg() << std::endl;
    }
    fixbug::RegisterRequest req;
    req.set_id(21);
    req.set_name("si");
    req.set_pwd("123");
    fixbug::RegisterResponse res;
    stub.Register(nullptr, &req, &res, nullptr);
    if (res.result().errcode() == 0) {
        std::cout << "rpc register response:" << res.success() << std::endl;
    }
    else {
        std::cout << "rpc register response error:" << res.result().errmsg() << std::endl;
    }
    return 0;
}