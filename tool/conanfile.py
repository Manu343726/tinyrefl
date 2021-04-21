import os
import re

from conans import CMake, ConanFile, tools
from conans.errors import ConanException

TINYREFL_BRANCH = os.environ.get('TINYREFL_BRANCH', 'refactoring-api')
TINYREFL_VERSION = os.environ.get('TINYREFL_VERSION', '0.5.6')
TINYREFL_TAG = 'v' + TINYREFL_VERSION

class TinyreflTool(ConanFile):
    name = 'tinyrefl-tool'
    version = TINYREFL_VERSION
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    scm = {
      'type': 'git',
      'url': 'https://gitlab.com/Manu343726/tinyrefl',
      'revision': TINYREFL_TAG,
      'subfolder': 'tinyrefl'
    }
    generators = 'cmake_find_package'
    options = {
        'jinja2_support': [True, False]
    }
    default_options = 'jinja2_support=True', 'fmt:header_only=False', 'protobuf:shared=False'
    settings = 'os', 'compiler', 'build_type', 'arch'

    custom_cmake_defs = {
        'TINYREFL_BUILD_TESTS': False,
        'TINYREFL_BUILD_EXAMPLES': False,
        'TINYREFL_GIT_BRANCH': TINYREFL_BRANCH,
        'TINYREFL_GIT_AT_TAG': True,
        'TINYREFL_GIT_LAST_TAG': TINYREFL_TAG
    }

    def requirements(self):
        self.requires('clang_executables/6.0.1@Manu343726/testing')

    def build_requirements(self):
        self.build_requires('ctti/0.0.2@Manu343726/testing')
        self.build_requires('cppast/master@Manu343726/testing')
        self.build_requires('llvm_support/6.0.1@Manu343726/testing')
        self.build_requires('type_safe/0.3@Manu343726/testing')
        self.build_requires('jsonformoderncpp/3.7.0')
        self.build_requires('protobuf/3.15.5')

        if self.options.jinja2_support:
            self.build_requires('jinja2cpp/1.1.0')
            self.build_requires('fmt/6.2.1')
            self.build_requires('spdlog/1.5.0')
        else:
            self.build_requires('fmt/7.1.3')
            self.build_requires('spdlog/1.8.5')

    def build(self):
        cmake = CMake(self)

        self.custom_cmake_defs.update({
            'TINYREFL_LLVM_VERSION': self.deps_cpp_info['libclang'].version,
            'TINYREFL_TOOL_JINJA2_SUPPORT': self.options.jinja2_support
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
