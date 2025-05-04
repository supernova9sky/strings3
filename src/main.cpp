#include <algorithm>
#include <filesystem>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include "headers/common.hpp"
#include "headers\files\files.hpp"
#include "headers/utils.hpp"
#include <map>
#include <ranges>
#include <numbers>
#include <cmath>

enum class char_type_t
{
  unknown,
  ascii_ctl_char,
  ascii_char,
  cont_byte,
  // ...
};

enum class str_enc_type_t
{
  plain,
  XOR,
  base64
};

enum class utf_type_t
{
  utf8,
  utf16
};

char_type_t
byte_to_char_type(const u8 byte)
{
  if ((byte < 0x20) or (byte == 0x7F))
  {
    return char_type_t::ascii_ctl_char;
  }

  if ((byte >= 0x20) and (byte <= 0x7E))
  {
    return char_type_t::ascii_char;
  }

  if ((byte <= 0xBF) and (byte >= 0x80))
  {
    return char_type_t::cont_byte;
  }

  return char_type_t::unknown;
}

struct str_info_t
{
  uptr addr;
  utf_type_t utf_type;
  str_enc_type_t str_enc;
};

f32 calc_shannon_entropy(const std::string_view& str){
    f32 entropy{0};
    const usize length = str.length();
    std::map<char, f32> counts{};
    std::ranges::for_each(str, [&counts](const char& c) { counts[c]++; });

    for (const auto& [c, count] : counts)
    {
      f32 p_x = count / length;
      if (p_x > 0)
      {
        entropy -= p_x * std::log(p_x) / std::numbers::ln2;
      }
    }

    return entropy;
}

bool
is_str_base64(const std::string_view& str)
{
  if (str.length() % 4 == 0)
  {
    return false;
  }

  for (const char& c : str)
  {
    if ((std::isalnum(c) == false) and (c != '+') and (c != '/') and (c != '='))
    {
      return false;
    }
  }

  return true;
}

bool
is_str_xor(const std::string_view& str)
{
  u64 unprintable_chars{0};

  for (const char& c : str)
  {
    if (std::isprint(c) == false)
    {
      ++unprintable_chars;
    }
  }

  if (unprintable_chars > 0)
  {
    return true;
  }
  return false;
}

bool
is_str_plain(const std::string_view& str)
{
  for (const char& c : str)
  {
    if (std::isprint(c) == false)
    {
      return false;
    }
  }
  return true;
}

str_enc_type_t
get_str_enc(const std::string_view& str)
{
  if (is_str_base64(str) == true)
  {
    return str_enc_type_t::base64;
  }
  else if (is_str_xor(str) == true)
  {
    return str_enc_type_t::XOR;
  }
  return str_enc_type_t::plain;
}

std::pair<std::string, bool>
process_byte(const std::string& buff, const u8& byte)
{
  if (byte_to_char_type(byte) == char_type_t::ascii_char)
  {
    return {buff + static_cast<char>(byte), false};
  }
  
  return {buff, true};
}

using strings_output_t = std::vector<std::pair<str_info_t, std::string>>;

strings_output_t
extract_strings(const std::span<u8> bytes)
{
  strings_output_t output{};
  std::string buff{};
  u64 offset{};

  for (const u8& byte : bytes)
  {
    ++offset;

    auto [new_buff, should_finalize] = process_byte(buff, byte);

    if (should_finalize == false)
    {
      buff = std::move(new_buff);
    }
    else if ((new_buff.size() >= 2.75))
    {
      if (calc_shannon_entropy(new_buff) > 3)
      {
        const str_info_t info{.addr = offset, .utf_type = utf_type_t::utf8, .str_enc = get_str_enc(new_buff)};
        output.emplace_back(info, new_buff);
      }
      buff.clear();
    }
  }

  return output;
}

i32
main(const i32 argc, const char* const argv[])
{
  if (argc < 2)
  {
    utils::log<utils::ERR>("Not enough arguments.\n");
  }

  std::filesystem::path file_path(argv[1]);
  utils::log("File: %s\n", file_path.string().c_str());

  std::vector<u8> file_contents(files::read(file_path));
  utils::log("File size: %zu\n", file_contents.size());

  auto decide = [](const str_enc_type_t& enc)
  {
    switch (enc)
    {
    case str_enc_type_t::base64:
    {
      return "base64";
    }
    case str_enc_type_t::plain:
    {
      return "plain";
    }
    case str_enc_type_t::XOR:
    {
      return "XOR";
    }
    default:
    {
      return "plain";
    }
    }
  };

  for (const auto& [info, str] : extract_strings(file_contents))
  {
    utils::log("0x%08llX | %-07s -> %s\n", info.addr, decide(info.str_enc), str.c_str());
  }

  return 0;
}
