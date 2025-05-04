project('string_parse', 'cpp',
  version : '0.1',
  default_options : [
    'buildtype=release',
    'default_library=shared',
    'debug=false',
    'warning_level=3', 'werror=true',
    'backend=ninja',
    'optimization=3',
  ]
)

source_files = run_command('python', 'globber.py', './', '*.cpp', '*.cxx', '*.cc', check: true).stdout().strip().split('\n')

message('source files: ')
foreach f : source_files
  message('   ' + f)
endforeach

executable('string_parse', [source_files],
  include_directories : ['./src'],
  link_args : ['-lntdll', '-static'],
  cpp_args: [
    # make use of the platform specific optimisations
    '-march=native', 
    '-mtune=native',

    # specify to build for the x64 architecture
    '-m64',

    # specify the newest c++ version
    '-std=c++2c',

    # specify to use intel syntax for inline assembly
    '-masm=intel',

    # specify to use the block syntax for inline assembly ("__asm {...}")
    '-fasm',
    
    # omit stackframe pointer savin (minor perf boost for small hot path function calls)
    '-fomit-frame-pointer',

    '-Wno-unused-variable',
    '-Wno-return-type',
    '-Wno-format-security'

    # disable warnings (this should only be used under warning_level=everything) (its impossible with zydis)
    #'-Wno-c++98-compat',
    #'-Wno-pre-c++23-compat',
    #'-Wno-pre-c++14-compat'
  ]
)
