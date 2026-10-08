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

#include "spdlog_helper.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::vector<spdlog::sink_ptr> sinks;

void spdlogInit()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("rsap.log", true);
    console_sink->set_level(spdlog::level::debug);
    file_sink->set_level(spdlog::level::info);

    sinks = {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("RSAP", sinks.begin(), sinks.end());
    logger->set_pattern("%Y-%m-%d %H:%M:%S [%n/%^%l%$] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}
