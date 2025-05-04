#include "strings.hpp"

#include "headers/utils.hpp"

namespace strings
{
std::pair<std::string, bool>
process_byte(const std::string& buff, const u8 byte)
{
  if (byte == 0x0)
  {
    return {buff, true};
  }

  if (utils::byte_to_char_type(byte) == utils::char_type_t::ascii_char)
  {
    return {buff + static_cast<char>(byte), false};
  }
  // else if ((utils::byte_to_char_type(byte) == utils::char_type_t::ascii_ctl_char) and
  //          (byte != '\a' and byte != '\v' and byte != '\b' and byte != '\n' and byte != '\r' and byte != '\0' and byte != '\x1F') and (buff.size() != 0))
  // {
  //   return {buff + static_cast<char>(byte), false};
  // }

  return {buff, true};
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
    else if ((new_buff.size() >= 3))
    {
      if (utils::calc_shannon_entropy(new_buff) > 3)
      {
        const str_info_t info{.addr = offset, .utf_type = utf_type_t::utf8, .str_enc = get_str_enc(new_buff)};
        output.emplace_back(info, new_buff);
      }
      buff.clear();
    }
  }

  return output;
}
} // namespace strings
