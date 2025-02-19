#include <iostream>
#include "rpcapp.h"
#include "friend.pb.h"
#include "rpcchannel.h"
#include "rpccontroller.h"

int main(int argc, char **argv) {
    RpcApp::Init(argc, argv);
    fixbug::FriendServiceRpc_Stub stub(new RpcChannel());
    fixbug::GetFriendsListRequest request;
    request.set_userid(1);
    fixbug::GetFriendsListResponse response;
    RpcController controller;

    stub.GetFriendsList(&controller, &request, &response, nullptr);

    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        if (response.result().errcode() == 0) {
            std::cout << "rpc login response:\n";
            for (auto s : response.friends()) std::cout << s << " ";
            std::cout << "\n";
        }
        else {
            std::cout << "rpc login response error:" << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}