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

#include "random_selector.h"
#include <algorithm>
#include <spdlog/spdlog.h>

void RandomSelector::setPersonList(const personVec& lst)
{
    m_personList = lst;
    buildPool();
}

const personVec &RandomSelector::getPersonList() const
{
    return m_personList;
}

void RandomSelector::setNoRepeat(bool enable)
{
    m_noRepeat = enable;
}

bool RandomSelector::getNoRepeat() const
{
    return m_noRepeat;
}

void RandomSelector::setSeed(uint32_t seed)
{
    m_gen.seed(seed);
    spdlog::info("Seed set: {}", seed);
}

void RandomSelector::resetNoRepeatPool()
{
    m_wtPool = m_pool;
}

void RandomSelector::buildPool()
{
    std::vector<int> t_pool;
    for (size_t i = 0; i < m_personList.size(); ++i) {
        for (int j = 0; j < m_personList[i].weight; j++) {
            t_pool.push_back(static_cast<int>(i));
        }
    }
    m_pool = t_pool;
    m_wtPool = t_pool;
}

int RandomSelector::randomPick()
{
    if (!m_noRepeat) {
        std::uniform_int_distribution<> dist(0, static_cast<int>(m_pool.size()) - 1);
        int rdd = m_pool[dist(m_gen)];
        spdlog::info("Random: {}", rdd);
        return rdd;
    } else {
        if (m_lastNoRepeat == false || m_wtPool.empty()) {
            m_wtPool = m_pool;
            spdlog::info("Copyed pool for no‑repeat");
        }
        std::uniform_int_distribution<> dist(0, static_cast<int>(m_wtPool.size()) - 1);
        int rdd = m_wtPool[dist(m_gen)];
        auto it = std::find(m_wtPool.begin(), m_wtPool.end(), rdd);
        if (it != m_wtPool.end()) {
            auto it2 = std::find_if(std::next(it), m_wtPool.end(), [rdd](int x) {
                return x != rdd;
            });
            if (it2 != m_wtPool.end())
                m_wtPool.erase(it, it2);
            else
                m_wtPool.erase(it, m_wtPool.end());
        }
        m_lastNoRepeat = m_noRepeat;
        spdlog::info("No Repeat Random: {}", rdd);
        return rdd;
    }
}
