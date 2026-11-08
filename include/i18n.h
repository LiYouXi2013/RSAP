#pragma once
#include <string>
#include <vector>

class i18n
{
private:
    std::string cuLang, cuLangName;
    std::vector<std::string> texts;

public:
    i18n(std::string selLang);
    std::string gt(unsigned int id);
};