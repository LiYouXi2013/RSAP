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

#pragma once
#include "utils.h"
#include <vector>
#include <random>
#include <cstdint>

class RandomSelector
{
public:
    RandomSelector() = default;

    void setPersonList(const personVec& lst);
    const personVec &getPersonList() const;

    void setNoRepeat(bool enable);
    bool getNoRepeat() const;

    void setSeed(uint32_t seed);
    int randomPick();
    void resetNoRepeatPool();

private:
    void buildPool();

    std::mt19937 m_gen{0};
    personVec m_personList;
    std::vector<int> m_pool;
    std::vector<int> m_wtPool;
    bool m_noRepeat = false;
    bool m_lastNoRepeat = false;
};
