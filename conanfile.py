import os
import re

from conans import python_requires, tools
from conans.errors import ConanException

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

TINYREFL_BRANCH = os.environ.get('TINYREFL_BRANCH', 'refactoring-api')
TINYREFL_VERSION = os.environ.get('TINYREFL_VERSION', '0.5.3')
TINYREFL_TAG = 'v' + TINYREFL_VERSION

class Tinyrefl(common.CMakePackage):
    name = 'tinyrefl'
    version = TINYREFL_VERSION
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    license = 'MIT'
    scm = {
        'type': 'git',
        'url': 'https://github.com/Manu343726/tinyrefl',
        'revision': TINYREFL_TAG,
        'subfolder': 'tinyrefl'
    }
    custom_cmake_defs = {
        'TINYREFL_BUILD_TESTS': False,
        'TINYREFL_BUILD_EXAMPLES': False,
        'TINYREFL_BUILD_TOOL': False,
        'TINYREFL_GIT_BRANCH': TINYREFL_BRANCH,
        'TINYREFL_GIT_AT_TAG': True,
        'TINYREFL_GIT_LAST_TAG': TINYREFL_TAG,
    }
    generators = 'cmake_find_package'
    requires = ('jsonformoderncpp/3.7.0',
               'ctti/0.0.2@Manu343726/testing')
    settings = 'os', 'compiler', 'build_type', 'arch'

    def package_info(self):
        super().package_info()

        if self.settings.compiler in ['gcc', 'clang']:
            self.cpp_info.cxxflags.append('-Wno-gnu-string-literal-operator-template')
