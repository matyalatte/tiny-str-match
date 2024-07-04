test_cpp_args = []
test_compiler = meson.get_compiler('c').get_id()
test_os = host_machine.system()
if test_os == 'windows'
    if test_compiler == 'msvc'
        test_cpp_args += ['/source-charset:utf-8']
    elif test_compiler == 'gcc'
        test_cpp_args += ['-finput-charset=UTF-8']
    endif
endif

test_exe = executable('uint_test',
    'main.cpp',
    cpp_args: test_cpp_args,
    dependencies : [tiny_str_match_dep, gtest_dep, gmock_dep],
    install : false)

test('unit_test', test_exe)
