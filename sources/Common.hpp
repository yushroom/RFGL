#ifndef Common_hpp
#define Common_hpp

#include <string>
#include <vector>

//========== string ==========//

std::vector<std::string> split(const std::string& str, const std::string& separator);

void trim(std::string& str);

bool startsWith(const std::string& str, const std::string& str2);

#endif