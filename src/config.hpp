#pragma once

#include <toml.hpp>

toml::table loadConfig(bool retry = true);