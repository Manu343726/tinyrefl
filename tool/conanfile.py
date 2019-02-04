from conans import ConanFile, CMake
import os

class TinyreflTool(ConanFile):
    name = 'tinyrefl-tool'
    version = '0.3.1'
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    scm = {
      'type': 'git',
      'url': 'https://github.com/Manu343726/tinyrefl',
      'revision': 'auto',
      'subfolder': 'tinyrefl'
    }
    generators = 'cmake'
    build_requires = ('jsonformoderncpp/3.5.0@vthiery/stable',
                'fmt/5.2.1@bincrafters/stable',
                'ctti/0.0.2@Manu343726/testing',
                'cppast/master@Manu343726/testing',
                'llvm_support/6.0.1@Manu343726/testing')
    requires = (('clang_executables/6.0.1@Manu343726/testing', 'private'),)
    default_options = 'fmt:header_only=True'
    settings = 'os', 'compiler', 'build_type', 'arch'

    def build(self):
        cmake = CMake(self)
        cmake.configure(
            source_folder='tinyrefl',
            defs = {
                'TINYREFL_BUILD_TESTS': False,
                'TINYREFL_BUILD_EXAMPLES': False
            }
        )
        cmake.build(target='tinyrefl-tool')

    def package(self):
        self.copy('tinyrefl-tool*',
            src='bin',
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
