#include <cctype>
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

  std::filesystem::path file_path(argv[1]);
  utils::log("File: %s\n", file_path.string().c_str());

  std::vector<u8> file_contents(files::read(file_path));
  utils::log("File size: %zu\n", file_contents.size());

  if (file_contents.size() == 0)
  {
    utils::log<utils::ERR>("File too small or doesn't exist.\n");
    return 2;
  }

  auto decide = [](const strings::str_enc_type_t& enc)
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
