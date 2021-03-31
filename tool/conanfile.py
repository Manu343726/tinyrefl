import os
import re

from conans import CMake, ConanFile, tools
from conans.errors import ConanException

TINYREFL_BRANCH = os.environ.get('TINYREFL_BRANCH', 'refactoring-api')
TINYREFL_VERSION = os.environ.get('TINYREFL_VERSION', '0.5.3')
TINYREFL_TAG = 'v' + TINYREFL_VERSION

class TinyreflTool(ConanFile):
    name = 'tinyrefl-tool'
    version = TINYREFL_VERSION
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    scm = {
      'type': 'git',
      'url': 'https://github.com/Manu343726/tinyrefl',
      'revision': TINYREFL_TAG,
      'subfolder': 'tinyrefl'
    }
    generators = 'cmake_find_package'
    build_requires = ('jsonformoderncpp/3.5.0@vthiery/stable',
                'ctti/0.0.2@Manu343726/testing',
                'cppast/master@Manu343726/testing',
                'llvm_support/6.0.1@Manu343726/testing',
                'type_safe/0.3@Manu343726/testing',
                'spdlog/1.3.1@bincrafters/stable',
                'fmt/5.3.0@bincrafters/stable',
                'jsonformoderncpp/3.5.0@vthiery/stable',
                'protobuf/3.15.5')
    requires = 'clang_executables/6.0.1@Manu343726/testing'
    default_options = 'fmt:header_only=True'
    settings = 'os', 'compiler', 'build_type', 'arch'

    custom_cmake_defs = {
        'TINYREFL_BUILD_TESTS': False,
        'TINYREFL_BUILD_EXAMPLES': False,
        'TINYREFL_GIT_BRANCH': TINYREFL_BRANCH,
        'TINYREFL_GIT_AT_TAG': True,
        'TINYREFL_GIT_LAST_TAG': TINYREFL_TAG
    }

    def build(self):
        cmake = CMake(self)

        self.custom_cmake_defs.update({
            'TINYREFL_LLVM_VERSION': self.deps_cpp_info['libclang'].version
        })

        cmake.configure(
            source_folder='tinyrefl',
            defs = self.custom_cmake_defs
        )
        cmake.build(target='tinyrefl-tool')

    def package(self):
        self.copy('tinyrefl-tool*',
            src='tool',
            dst='bin')

        self.copy('utils.cmake',
            src=os.path.join(self.source_folder, 'tinyrefl', 'cmake'),
            dst='cmake',
            keep_path=False)

        self.copy('driver.cmake',
            src=os.path.join(self.source_folder, 'tinyrefl', 'tool'),
            dst='cmake',
            keep_path=False)

        self.copy('tinyrefl_tool-config.cmake',
            src=os.path.join(self.source_folder, 'tinyrefl', 'cmake'),
            dst='cmake',
            keep_path=False)

        self.copy('tinyrefl_tool-version.cmake',
            dst='cmake',
            keep_path=False)
