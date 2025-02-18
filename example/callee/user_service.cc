#include <iostream>
#include <string>
#include "user.pb.h"
#include "rpcapp.h"
#include "rpcprovider.h"

class UserService : public fixbug::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service : Login " << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
    const ::fixbug::LoginRequest* request,
    ::fixbug::LoginResponse* response,
    ::google::protobuf::Closure* done) {
        std::string name = request->name();
        std::string pwd = request->pwd();
        std::cout << name << " : " << pwd << std::endl;
        bool login_result = Login(name, pwd);
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        done->Run();
    }
};

int main(int argc, char **argv) {
    RpcApp::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.Run();
    return 0;
}