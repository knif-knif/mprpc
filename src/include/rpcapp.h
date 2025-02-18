#pragma once

#include "rpcconfig.h"

class RpcApp {
public:
    static void Init(int argc, char **argv);
    static RpcApp &GetInstance();
    static RpcConfig &GetConfig();
private:
    static RpcConfig m_config;
    RpcApp(){}
    RpcApp(const RpcApp &) = delete;
    RpcApp(RpcApp &&) = delete;
};