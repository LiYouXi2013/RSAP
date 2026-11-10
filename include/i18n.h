#pragma once
#include <string>
#include <vector>

std::string getLangName(std::string lang);

class i18n
{
private:
    std::vector<std::string> texts;

public:
    i18n(std::string selLang);
    std::string gt(unsigned int id);
};