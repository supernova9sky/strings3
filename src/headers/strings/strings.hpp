#pragma once

#include <vector>
#include <string>
#include <span>

#include "headers/common.hpp"


namespace strings
{
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

struct str_info_t
{
  uptr addr;
  utf_type_t utf_type;
  str_enc_type_t str_enc;
};

std::pair<std::string, bool> process_byte(const std::string& buff, const u8& byte);

std::vector<std::pair<str_info_t, std::string>> extract_strings(const std::span<u8> bytes);

}
