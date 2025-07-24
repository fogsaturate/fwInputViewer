#pragma once

#include <toml++/toml.hpp>

toml::table loadConfig(bool retry = true);