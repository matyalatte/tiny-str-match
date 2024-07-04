project('tiny-str-match', 'c', version: '0.1.0')

# set source files
tsm_sources = [
    'src/wildcard.c',
    'src/utf.c',
    'src/re.c',
]

# dependency for other projects
tiny_str_match_dep = declare_dependency(
    include_directories: include_directories('./include'),
    sources : tsm_sources)

# Build unit tests
if get_option('tests')
    add_languages('cpp', native:false, required: true)

    # get gtest
    gtest_proj = subproject('gtest')
    gtest_dep = gtest_proj.get_variable('gtest_dep')
    gmock_dep = gtest_proj.get_variable('gmock_dep')

    # build tests
    subdir('tests')
endif
