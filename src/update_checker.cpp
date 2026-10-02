#include "update_checker.h"/*
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


#include <FL/fl_ask.H>

void UpdateChecker::awakeCb(void* ud)
{
    auto* self = reinterpret_cast<UpdateChecker *>(ud);
    if (!self->info.ok) {
        fl_message("Cannot check update!\nMet an error!");
    } else {
        if (!self->info.newer) {
            fl_message("All Up To Date!");
        } else {
            if (fl_ask("Found a newer version: %llu \n "
                       "Would you like to open the release page? \n"
                       "  Password: 1234", self->info.remoteVer)) {
                system("start https://lyx201312.lanzouu.com/b00wnw20ri");
            }
        }
    }
    self->doing = false;
}

void UpdateChecker::workThread()
{
    DWORD err = 0;
    std::string body = httpGet(L"updatechecker.pages.dev", L"/rsap/version.txt", err);
    if (body.empty()) {
        info.ok = false;
        Fl::awake(awakeCb, this);
        return;
    }
    std::string text = trim(body);
    long long remoteBuild = 0;
    if (!toInt(text, remoteBuild)) {
        info.ok = false;
        Fl::awake(awakeCb, this);
        return;
    }
    info.remoteVer = static_cast<unsigned long long>(remoteBuild);
    info.newer = (remoteBuild > static_cast<long long>(info.localVer));
    info.ok = true;
    Fl::awake(awakeCb, this);
}

void UpdateChecker::backgroundCheck()
{
    if (doing) {
        fl_message("Already Checking!");
        return;
    }
    doing = true;
    std::thread t(&UpdateChecker::workThread, this);
    t.detach();
}
