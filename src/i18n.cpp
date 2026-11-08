#include "i18n.h"
#include "utils.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

i18n::i18n(std::string selLang)
{
    auto l = get_logger("worker.i18n");

    json j;
    {
        std::ifstream f("./lang/" + selLang + ".json");
        if (!f.is_open()) {
            l->warn("Language not found: {}", selLang);
            freopen(std::string("./lang/" + selLang + ".json").c_str(), "w", stdout);
            std::cout << '\n';
            fclose(stdout);
        }

        try {
            f >> j;
        } catch (const std::exception& e) {
            l->error("Failed to parse lang file {}: {}", selLang, e.what());
            j = json::object();
        }
    }

    cuLangName = j.value("LangName", "NoName");

    if (j.is_object() && j.contains("Translations") && j["Translations"].is_array()) {
        const auto& transArray = j["Translations"];
        texts.reserve(transArray.size());

        for (const auto& trans : transArray) {
            texts.push_back(trans.get<std::string>());
        }
    }
}

std::string i18n::gt(unsigned int id)
{
    try {
        return texts.at(id);
    } catch (...) {
        std::cout << "No ID:" << id << std::endl;
        return "Id <" + std::to_string(id) + ">";
    }
}