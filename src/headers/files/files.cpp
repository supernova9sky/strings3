#include "files.hpp"
#include <filesystem>
#include <fstream>
#include <ios>

namespace files
{

std::string
error_to_str(const error_t err)
{
  switch (err)
  {
  case error_t::filesize_threw:
  {
    return "filesize threw";
  }
  case error_t::filesize_is_zero:
  {
    return "filesize is zero";
  }
  case error_t::ifstream_error:
  {
    return "ifstream error";
  }
  case error_t::ifstream_fatal_error:
  {
    return "ifstream fatal error";
  }
  }
}

std::expected<std::vector<u8>, error_t>
read(const std::filesystem::path& file_path)
{
  std::ifstream stream(file_path, std::ios::in);

  if (stream.fail() == true)
  {
    return std::unexpected(error_t::ifstream_error);
  }

  if (stream.bad() == true)
  {
    return std::unexpected(error_t::ifstream_fatal_error);
  }

  usize file_size{};

  try
  {
    file_size = std::filesystem::file_size(file_path);
  }
  catch (...)
  {
    return std::unexpected(error_t::filesize_threw);
  }

  if (file_size == 0)
  {
    return std::unexpected(error_t::filesize_is_zero);
  }

  std::vector<u8> read_file(file_size);
  stream.read(reinterpret_cast<char*>(read_file.data()), file_size);

  return read_file;
}
} // namespace files
