#include "rpcapp.h"
#include <iostream>
#include <unistd.h>

RpcConfig RpcApp::m_config;

void ShowArgsHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

void RpcApp::Init(int argc, char **argv) {
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c) {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
            break;
        case ':':
            std::cout << "need <configfile>" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    // Load config file
    m_config.LoadConfigFile(config_file.c_str());
}

RpcApp& RpcApp::GetInstance() {
    static RpcApp app;
    return app;
}

RpcConfig& RpcApp::GetConfig() {
    return m_config;
}