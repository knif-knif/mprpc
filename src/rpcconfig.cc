#include "rpcconfig.h"
#include <iostream>
#include <string>

void RpcConfig::LoadConfigFile(const char *config_file) {
    FILE *pf = fopen(config_file, "r");
    if (nullptr == pf) {
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 1.注释 2.正确的配置项 3.去除多余空格
    while (!feof(pf)) {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        std::string read_buf(buf);
        Trim(read_buf);
        
        if (read_buf[0] == '#' || read_buf.empty()) continue;
        int idx = read_buf.find('=');
        if (idx == -1) {
            continue;
        }
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        value = read_buf.substr(idx + 1, read_buf.size() - idx);
        Trim(value);
        m_configMap.insert({key, value});
    }
}

std::string RpcConfig::Load(const std::string &key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) return "";
    return it->second;
}

void RpcConfig::Trim(std::string &src_buf) {
    int idx = src_buf.find('\n', 0);
    if (idx != -1) {
        src_buf = src_buf.substr(0, idx);
    }
    idx = src_buf.find_first_not_of(' ');
    if (idx != -1) {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1) {
        src_buf = src_buf.substr(0, idx + 1);
    }
    // if (f) Trim(src_buf);
}