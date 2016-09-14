#include "Common.hpp"

std::vector<std::string> split(const std::string& str, const std::string& separator){
    auto l = separator.length();
    auto pos = str.find_first_of(separator);
    decltype(pos) last_pos = 0;
    std::vector<std::string> ret;
    while (pos != std::string::npos) {
        ret.push_back(str.substr(last_pos, pos-last_pos));
        pos += l;
        last_pos = pos;
        pos = str.find_first_of(separator, pos);
    }
    ret.push_back(str.substr(last_pos, pos));
    return ret;
}

void trim(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ')+1);
}

bool startsWith(const std::string& str, const std::string& str2) {
    return str.substr(0, str2.size()) == str2;
}
