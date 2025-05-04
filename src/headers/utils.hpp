#pragma once
#include "common.hpp"
#include <utility>
#include <vector>
#include <windows.h>
#include <string>
#include <winternl.h>

namespace utils
{
enum log_status_t : u8
{
  OK,
  ERR
};

template <log_status_t status = OK, unsigned short text_colour = 7>
inline u0
log(const std::string_view& fmt, auto&&... args)
{
  const static HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  std::printf("[");
  switch (status)
  {
  case OK:
  {
    SetConsoleTextAttribute(std_handle, 10);
    std::printf("   OK   ");
    break;
  }
  case ERR:
  {
    SetConsoleTextAttribute(std_handle, 12);
    std::printf(" FAILED ");
    break;
  }
  }
  SetConsoleTextAttribute(std_handle, text_colour);
  std::printf("] ");

  std::printf(fmt.data(), std::forward<decltype(args)>(args)...);
}

inline HMODULE
get_or_load_module(const std::string& name)
{
  HMODULE result = GetModuleHandleA(name.c_str());
  return (result != nullptr) ? result : LoadLibraryA(name.c_str());
}

template <typename Def_t>
inline static Def_t
find_function(const std::string_view& module_name, const std::string_view& function_name)
{
  using module_info = std::pair<std::string, HMODULE>;
  static std::vector<module_info> modules_vec{};
  HMODULE hmod{nullptr};

  for (const auto& [name, value] : modules_vec)
  {
    if (name == module_name.data())
    {
      hmod = value;
    }
  }
  if (hmod == nullptr)
  {
    hmod = modules_vec.emplace_back(module_name, get_or_load_module(module_name.data())).second;
  }

  log("searching for %s in module %s at 0x%llX\n", function_name.data(), module_name.data(), hmod);
  return reinterpret_cast<Def_t>(reinterpret_cast<u0*>(GetProcAddress(hmod, function_name.data())));
}

inline bool
adjust_privilege(const ul privilege_number, const bool should_enable)
{
  using RtlAdjustPrivilege_t = NTSTATUS (*)(ul Privilege, bool Enable, bool Client, bool* WasEnabled);
  auto RtlAdjustPrivilege    = find_function<RtlAdjustPrivilege_t>("ntdll.dll", "RtlAdjustPrivilege");
  if (RtlAdjustPrivilege == nullptr)
  {
    log<utils::ERR>("couldn't locate RtlAdjustPrivilege\n");
    return false;
  }
  log("found RtlAdjustPrivilege at 0x%llX\n", RtlAdjustPrivilege);

  bool previous_state{};
  NTSTATUS status = RtlAdjustPrivilege(privilege_number, should_enable, false, &previous_state);

  if (NT_SUCCESS(status) == false)
  {
    log<utils::ERR>("RtlAdjustPrivilege failed, error code 0x%lX\n", status);
    return false;
  }
  return true;
}

inline bool
execute_syscall(const auto&&... args [[maybe_unused]], const u32 syscall_number)
{
  // TODO
}
} // namespace utils
