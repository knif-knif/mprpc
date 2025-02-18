#include <iostream>
#include <string>
#include "test.pb.h"

using namespace fixbug;

int main() {
    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("null");
    User *user1 = rsp.add_friend_list();
    user1->set_name("skn");
    user1->set_age(100);
    user1->set_sex(User::MAN);
    std::cout << rsp.friend_list_size() << std::endl;
    
    return 0;
}