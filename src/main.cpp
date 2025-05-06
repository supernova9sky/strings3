#include <filesystem>

#include "headers/common.hpp"
#include "headers/files/files.hpp"
#include "headers/utils.hpp"
#include "headers/strings/strings.hpp"

i32
main(const i32 argc, const char* const argv[])
{
  if (argc < 2)
  {
    utils::log<utils::ERR>("Not enough arguments.\n");
    return 1;
  }

  const std::filesystem::path file_path(argv[1]);
  utils::log("File: %s.\n", file_path.string().c_str());

  const std::expected<std::vector<u8>, files::error_t> read_result = files::read(file_path);
  if (read_result.has_value() == false)
  {
    utils::log<utils::ERR>("Failed to read the file due to the following reason: %s.\n", files::error_to_str(read_result.error()).c_str());
    return 2;
  }

  const std::vector<u8>& file_contents(read_result.value());
  utils::log("File size: %zu.\n", file_contents.size());

  const auto decide = [](const strings::str_enc_type_t& enc)
  {
    switch (enc)
    {
    case strings::str_enc_type_t::base64:
    {
      return "base64";
    }
    case strings::str_enc_type_t::plain:
    {
      return "plain";
    }
    case strings::str_enc_type_t::XOR:
    {
      return "xor";
    }
    default:
    {
      return "plain";
    }
    }
  };

  for (const auto& [info, str] : strings::extract_strings(file_contents))
  {
    utils::log("0x%08llX | %-7s -> %s\n", info.addr, decide(info.str_enc), str.c_str());
  }

  return 0;
}
