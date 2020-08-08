from conans import ConanFile, CMake, tools
from conans.errors import ConanException
import os, re

class Git(tools.Git):

    def get_last_tag(self):
        self.check_repo()
        try:
            status = self.run("describe --abbrev=0")
            tag = status.strip()
            return tag
        except Exception:
            return None

def get_tag_version(git):
    tag = git.get_tag()

    if tag is None:
        return None

    result = re.fullmatch(r'v([0-9]+)\.([0-9]+)\.([0-9]+)', tag)

    if result is None:
        raise ConanException('Invalid git tag "{}"'.format(tag))

    return result.expand(r'\1.\2.\3')

class TinyreflTool(ConanFile):
    name = 'tinyrefl-tool'
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    scm = {
      'type': 'git',
      'url': 'https://github.com/Manu343726/tinyrefl',
      'revision': 'auto',
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
                'jsonformoderncpp/3.5.0@vthiery/stable')
    requires = 'clang_executables/6.0.1@Manu343726/testing'
    default_options = 'fmt:header_only=True'
    settings = 'os', 'compiler', 'build_type', 'arch'

    custom_cmake_defs = {
        'TINYREFL_BUILD_TESTS': False,
        'TINYREFL_BUILD_EXAMPLES': False
    }

    def set_version(self):
        git = Git(self.recipe_folder)
        tag_version = get_tag_version(git)

        if tag_version is not None:
            self.version = tag_version
        else:
            self.version = '{}-{}'.format(git.get_branch(), git.run('rev-parse --short HEAD'))

        self.custom_cmake_defs.update({
            'TINYREFL_GIT_BRANCH': git.get_branch(),
            'TINYREFL_GIT_COMMIT': git.get_commit(),
            'TINYREFL_GIT_AT_TAG': (tag_version is not None),
            'TINYREFL_LAST_TAG': git.get_last_tag()
        })

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
