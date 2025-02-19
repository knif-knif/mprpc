#include <iostream>
#include <string>
#include "rpcapp.h"
#include "rpcprovider.h"
#include "friend.pb.h"
#include <vector>

class FriendService : public fixbug::FriendServiceRpc {
public:
    std::vector<std::string> GetFriendsList(uint32_t userid) {
        std::cout << "do GetFriendsList service!";
        std::vector<std::string> vec;
        vec.push_back("a");
        vec.push_back("B");
        vec.push_back("CC");
        vec.push_back("ddd");
        return vec;
    }

    void GetFriendsList(::google::protobuf::RpcController* controller,
                        const ::fixbug::GetFriendsListRequest* request,
                        ::fixbug::GetFriendsListResponse* response,
                        ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendsList = GetFriendsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendsList) {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char** argv) {
    RpcApp::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.Run();

    return 0;
}