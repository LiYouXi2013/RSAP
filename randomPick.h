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

#include <vector>
#include <random>
#include <string>
#include <algorithm>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std;

static mt19937 gen(0);

struct Person {
    string name = "无名氏";
    int weight = 1;
};

vector<int> pool;
vector<int> wt_pool;
bool is_norepeat = false;
bool lnr = false;

void buildPool(const vector<Person> &lst)
{
    vector<int> t_pool;
    for (size_t i = 0; i < lst.size(); ++i) {
        for (int j = 0; j < lst[i].weight; ++j) {
            t_pool.push_back(i);
        }
    }
    pool = t_pool;
    wt_pool = t_pool;
}

int randomPick()
{
    if (!is_norepeat) {
        std::uniform_int_distribution<> dist(0, (int)pool.size() - 1);
        lnr = is_norepeat;
        int rdd = pool[dist(gen)];
        spdlog::info("Random: {}", rdd);
        return rdd;
    } else {
        if (lnr == false || wt_pool.empty()) {
            wt_pool = pool;
            spdlog::info("Copyed pool");
        }
        std::uniform_int_distribution<> dist(0, (int)wt_pool.size() - 1);
        int rdd = wt_pool[dist(gen)];
        auto it = find(wt_pool.begin(), wt_pool.end(), rdd);

        if (it != wt_pool.end()) {
            size_t idx = it - wt_pool.begin();
            auto it2 = find_if(next(it), wt_pool.end(), [rdd](int x) {
                return x != rdd;
            });

            if (it2 != wt_pool.end()) {
                wt_pool.erase(it, it2);
            } else {
                wt_pool.erase(it, wt_pool.end());
            }
        }
        lnr = is_norepeat;
        spdlog::info("No Repeat Random: {}", rdd);
        return rdd;
    }
}

typedef vector<Person> personVec;
