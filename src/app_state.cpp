/*
RSAP
Copyright (C) 2026 LiYouXi2013, Candyman_RDFZ

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "app_state.h"
#include <windows.h>
#include <random>

#include "utils.h"

bool AppState::loadFromIni(const std::string& iniPath)
{
    auto l = get_logger("worker.appstate");

    char t1[65536] {};
    seed = static_cast<uint32_t>(GetPrivateProfileIntA("General", "Seed", 0, iniPath.c_str()));
    ast = GetPrivateProfileIntA("General", "AST", 0, iniPath.c_str());

    GetPrivateProfileStringA("General", "Weight", "TESTNAME-1", t1, sizeof(t1), iniPath.c_str());
    persons = readInt(std::string(t1));

    for (const auto& p : persons) {
        l->debug("Name: {} Weight: {}", p.name, p.weight);
    }

    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
        isDefaultSeed = true;
        l->debug("Used Def Seed");
    }
    l->info("Seed: {}", seed);
    return true;
}

bool AppState::saveToIni(const std::string& iniPath)
{
    auto l = get_logger("worker.appstate");

    if (isDefaultSeed) {
        WritePrivateProfileStringA("General", "Seed", "0", iniPath.c_str());
        l->debug("Used Def Seed");
    } else {
        WritePrivateProfileStringA("General", "Seed", std::to_string(seed).c_str(), iniPath.c_str());
    }
    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
    }
    WritePrivateProfileStringA("General", "AST", std::to_string(static_cast<uint32_t>(ast)).c_str(), iniPath.c_str());
    WritePrivateProfileStringA("General", "Weight", join(persons).c_str(), iniPath.c_str());
    l->info("All settings saved");
    return true;
}
