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

using namespace std;

static mt19937 gen(0);

struct Person {
    string name = "无名氏";
    int weight = 1;
};

vector<int> buildPool(const vector<Person> &lst)
{
    vector<int> pool;
    for (size_t i = 0; i < lst.size(); ++i) {
        for (int j = 0; j < lst[i].weight; ++j) {
            pool.push_back(i);
        }
    }
    return pool;
}

int randomPick(const vector<int> &pool)
{
    std::uniform_int_distribution<> dist(0, (int)pool.size() - 1);
    return pool[dist(gen)];
}

typedef vector<Person> personVec;
typedef vector<int> poolVec;
