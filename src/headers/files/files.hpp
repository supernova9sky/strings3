#pragma once
#include <filesystem>
#include <vector>
#include "headers/common.hpp"

namespace files
{

std::vector<u8>
read(const std::filesystem::path& file_path);

}
