#include "strings.hpp"

#include <algorithm>
#include <ranges>
#include <string>
#include <ranges>

#include "headers/utils.hpp"

namespace strings
{
namespace
{
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

// bool
// is_str_plain(const std::string_view& str)
// {
//   for (const char& c : str)
//   {
//     if (std::isprint(c) == false)
//     {
//       return false;
//     }
//   }
//   return true;
// }

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

std::string
remove_whitespaces(const std::string_view& str, const std::string_view& whitespaces = " ")
{
  static const auto isnt_whitespace = [&whitespaces](const char& c) -> bool { return whitespaces.contains(c) == false; };

  std::string s{str};

  s.erase(std::ranges::find_if(std::ranges::reverse_view(s), isnt_whitespace).base(), s.end());
  s.erase(s.begin(), std::ranges::find_if(s, isnt_whitespace));

  return s;
}

std::optional<std::pair<str_info_t, std::string>>
make_string_info(const std::string_view& str, const u64& str_offset)
{
  if ((str.length() > 3) and (utils::calc_shannon_entropy(str) > 3))
  {
    const str_info_t info{.addr = str_offset, .utf_type = utf_type_t::utf8, .str_enc = get_str_enc(str)};
    return std::pair<str_info_t, std::string>{info, str};
  }
  return std::nullopt;
}

char
process_byte(const u8& byte)
{
  if (utils::byte_to_char_type(byte) == utils::char_type_t::ascii_char)
  {
    return static_cast<char>(byte);
  }

  return 0;
}
} // namespace

std::vector<std::pair<str_info_t, std::string>>
extract_strings(std::span<const u8> bytes)
{
  std::vector<std::pair<str_info_t, std::string>> output{};
  static const auto add_str_to_output = [&](const auto& a) { return output.emplace_back(std::move(a)); };

  u64 file_offset{0};
  u64 str_offset{0};
  for (std::string str{""}; const u8& byte : bytes)
  {
    const char new_char = process_byte(byte);
    ++file_offset;

    if (new_char != 0)
    {
      str_offset = file_offset;
      str += new_char;
      continue;
    }

    str = remove_whitespaces(str);
    make_string_info(str, str_offset - str.length()).transform(add_str_to_output);
    str.clear();
    str_offset = 0;
  }

  return output;
}
} // namespace strings
