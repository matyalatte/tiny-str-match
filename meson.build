project('tiny-str-match', 'c',
    meson_version: '>=0.54.0',
    default_options: [
        'warning_level=3',              # always max warnings
        'c_std=c99',                    # strict C99
        'cpp_std=c++11',                # strict C++11
    ],
    version: '0.1.0')

# set source files
tsm_sources = [
    'src/wildcard.c',
    'src/utf.c',
    'src/re.c',
]

if meson.version().version_compare('>=1.3.0')
    tiny_str_match_lib = library('tiny_str_match',
        tsm_sources,
        c_static_args: ['-D_TSM_STATIC'],
        install: true,
        include_directories: include_directories('./include'),
        gnu_symbol_visibility: 'hidden')
else
    # TODO: Remove this else block to support only meson 1.3.0 or later.
    tsm_c_args = []
    if get_option('default_library') == 'both'
        error('tiny-str-match requires meson 1.3.0 or later to build both shared and static libraries at the same time')
    elif get_option('default_library') == 'static'
        tsm_c_args = ['-D_TSM_STATIC']
    endif
    tiny_str_match_lib = library('tiny_str_match',
        tsm_sources,
        c_args: tsm_c_args,
        install: true,
        include_directories: include_directories('./include'),
        gnu_symbol_visibility: 'hidden')
endif

# dependency for other projects
tiny_str_match_dep = declare_dependency(
    include_directories: include_directories('./include'),
    link_with: tiny_str_match_lib)

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
