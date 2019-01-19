from conans import python_requires
import os

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

class Tinyrefl(common.CMakePackage):
    name = 'tinyrefl'
    version = '0.3.0'
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
        'TINYREFL_BUILD_EXAMPLES': False
    }
    generators = 'cmake'
    requires = ('jsonformoderncpp/3.5.0@vthiery/stable',
                'fmt/5.2.1@bincrafters/stable',
                'ctti/0.0.2@Manu343726/testing',
                ('cppast/master@Manu343726/testing', 'private'),
                ('llvm_support/6.0.1@Manu343726/testing', 'private'))
    default_options = 'fmt:header_only=True'
    settings = 'os', 'compiler', 'build_type', 'arch'

    def package(self):
        super().package()

        self.copy('tinyrefl-tool*',
            src='bin',
            dst='bin')

        self.copy('utils.cmake',
            src=os.path.join(self._repository_path, 'cmake'),
            dst='cmake',
            keep_path=False)

        self.copy('driver.cmake',
            src=os.path.join(self._repository_path, 'tool'),
            dst='cmake',
            keep_path=False)

        self.copy('tinyrefl_tool-config.cmake',
            src=os.path.join(self._repository_path, 'cmake'),
            dst='cmake',
            keep_path=False)

        self.copy('tinyrefl_tool-version.cmake',
            dst='cmake',
            keep_path=False)
