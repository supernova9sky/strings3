#include "files.hpp"
#include <filesystem>
#include <fstream>
#include <ios>

namespace files
{
std::vector<u8>
read(const std::filesystem::path& file_path)
{
  std::ifstream stream(file_path, std::ios::in);

  const usize file_size = std::filesystem::file_size(file_path);

  std::vector<u8> read_file(file_size);
  stream.read(reinterpret_cast<char*>(read_file.data()), file_size);

  return read_file;
}
}
