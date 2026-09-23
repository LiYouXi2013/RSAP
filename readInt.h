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
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

struct Person;

vector<Person> readInt(const string& t)
{
    auto l = t.begin();
    while (l != t.end() && std::isspace(*l)) ++l;
    auto r = t.end();
    do {
        --r;
    } while (std::distance(l, r) > 0 && std::isspace(*r));
    string s = string(l, r + 1);

    std::vector<Person> res;
    std::stringstream ss(s);
    std::string item;
    string t1;
    string t2;

    while (std::getline(ss, item, ',')) {
        stringstream t3(item);
        getline(t3, t1, '-');
        getline(t3, t2, '-');

        int val;
        try {
            val = std::stoi(t2);
        } catch (...) {
            continue;
        }

        res.push_back({t1, val});
    }

    if (res.size() == 0)printf("nmd");

    return res;
}

std::string join(const std::vector<Person> &v)
{
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ',';
        s += v[i].name;
        s += '-';
        s += to_string(v[i].weight);

    }
    return s;
}