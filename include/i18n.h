#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

#include "utils.h"

using json = nlohmann::json;

class i18n{
    private:
        std::string cuLang,cuLangName;
        std::vector<std::string> texts;

    public:
        i18n(std::string selLang){
            auto l = get_logger("worker.i18n");

            json j;
            {
                std::ifstream f("./lang/"+selLang+".json");
                if (!f.is_open()) {
                    l->warn("Language not found: {}", selLang);
                    freopen("settings.json", "w", stdout);
                    std::cout<<'\n';
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

        std::string gt(unsigned int id){
            try{
                return texts.at(id);
            }catch(...){
                return "<Id " + std::to_string(id) + "> not found!";
            }
        }
};