from conans import python_requires
import os

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

class Tinyrefl(common.CMakePackage):
    name = 'tinyrefl'
    version = '0.4.0'
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    scm = {
      'type': 'git',
      'url': 'https://github.com/Manu343726/tinyrefl',
      'revision': 'auto',
      'subfolder': 'tinyrefl'
    }
    custom_cmake_defs = {
        'TINYREFL_BUILD_TESTS': False,
        'TINYREFL_BUILD_EXAMPLES': False,
        'TINYREFL_BUILD_TOOL': False
    }
    generators = 'cmake'
    requires = ('jsonformoderncpp/3.5.0@vthiery/stable',
               'ctti/0.0.2@Manu343726/testing')
    settings = 'os', 'compiler', 'build_type', 'arch'
