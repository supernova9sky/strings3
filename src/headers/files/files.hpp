#pragma once

#include <filesystem>
#include <vector>
#include <expected>

#include "headers/common.hpp"

namespace files
{
enum class error_t : u8
{
  filesize_threw,
  filesize_is_zero,
  ifstream_error,
  ifstream_fatal_error
};

std::string error_to_str(const error_t err);

std::expected<std::vector<u8>, error_t> read(const std::filesystem::path& file_path);
}
