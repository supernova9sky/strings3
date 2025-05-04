#pragma once

#include "common.hpp"

#include <map>
#include <cmath>
#include <string>
#include <utility>
#include <numbers>
#include <algorithm>

namespace utils
{
enum log_status_t : u8
{
  OK,
  ERR
};

enum class char_type_t
{
  unknown,
  ascii_ctl_char,
  ascii_char,
  cont_byte,
  /* TODO add handling for utf-16 and multibyte utf-8 */
};

template <log_status_t status = OK, unsigned short text_colour = 7>
inline u0
log(const std::string_view& fmt, auto&&... args)
{
  std::printf("[");
  switch (status)
  {
  case OK:
  {
    std::printf("\033[0;92m   OK   \033[0m");
    break;
  }
  case ERR:
  {
    std::printf("\033[0;91m FAILED \033[0m");
    break;
  }
  }
  std::printf("] ");

  std::printf(fmt.data(), std::forward<decltype(args)>(args)...);
}

inline
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

inline
f32
calc_shannon_entropy(const std::string_view& str)
{
  f32 entropy{0};
  const usize length = str.length();
  std::map<char, f32> counts{};
  std::ranges::for_each(str, [&counts](const char& c) { counts[c]++; });

  for (const auto& [c, count] : counts)
  {
    const f32 p_x = count / length;
    if (p_x > 0)
    {
      entropy -= p_x * std::log(p_x) / std::numbers::ln2;
    }
  }

  return entropy;
}

} // namespace utils
