#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <string>
#include <random>

#include "randomPick.h"
#include "readInt.h"

class AppState
{
public:
    void setPerson(int id, std::string name = "UNSET", int weight = -1)
    {
        bool setname = true;
        bool setweight = true;

        if (name == "UNSET") {
            setname = false;
        }
        if (weight == -1) {
            setweight = false;
        }

        if (setname || setweight) {} else {
            spdlog::warn("AppState: setPerson: Nothing to set!");
        }

        if (id < 0) {
            spdlog::error("AppState: setPerson: ID<0!");
            return;
        }

        if (id >= persons.size()) {
            id = persons.size() + 1;
            persons.push_back({"", 0});
        }

        if (setname) {
            persons[id - 1].name = name;
        }

        if (setweight) {
            persons[id - 1].weight = weight;
        }

        buildPool(persons);
    }

    void setSeed(int sd = 0)
    {
        if (sd == 0) {
            static std::random_device rd;
            sd = rd();
            seed_isdef = true;
            spdlog::debug("Used Def Seed");
        } else {
            seed_isdef = false;
        }
        seed = sd;
        spdlog::info("Seed: {}", seed);
        gen.seed(seed);
    }

    int getSeed(bool forConfig)
    {
        if (forConfig && seed_isdef) {
            return 0;
        } else {
            return seed;
        }
    }

    bool autoStopEnable() const
    {
        return autostop;
    }
    int autoStopDuration() const
    {
        return autostop_timelength;
    }

    void setAutoStopEnable(bool b)
    {
        autostop = b;
    }
    void setAutoStopDuration(int msec)
    {
        if (msec >= 0) {
            autostop_timelength = msec;
        }
    }

    void loadConfig()
    {
        seed = GetPrivateProfileIntA("General", "Seed", 0, "./settings.ini");
        autostop_timelength = GetPrivateProfileIntA("General", "AST", 0, "./settings.ini");

        char t1[65536];
        GetPrivateProfileStringA("General", "Weight", "TESTNAME-1", t1, sizeof(t1), "./settings.ini");

        string t_str = t1;
        persons = readInt(t_str);

        setSeed(seed);

        spdlog::info("Inited settings");
    }

    void saveConfig()
    {
        WritePrivateProfileStringA("General", "Seed", to_string(getSeed(true)).c_str(), "./settings.ini");
        WritePrivateProfileStringA("General", "AST", to_string(autostop_timelength).c_str(), "./settings.ini");
        WritePrivateProfileStringA("General", "Weight", join(persons).c_str(), "./settings.ini");

        spdlog::info("All settings saved");
    }

private:
    personVec persons;
    int seed;
    bool seed_isdef = false;

    int autostop_timelength;
    bool autostop = false;
};