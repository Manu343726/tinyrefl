from conans import python_requires, tools
from conans.errors import ConanException
import os, re

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

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

class Tinyrefl(common.CMakePackage):
    name = 'tinyrefl'
    url = 'https://github.com/Manu343726/tinyrefl'
    description = ' A work in progress minimal C++ static reflection API and codegen tool'
    license = 'MIT'
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
    generators = 'cmake_find_package'
    requires = ('jsonformoderncpp/3.5.0@vthiery/stable',
               'ctti/0.0.2@Manu343726/testing')
    settings = 'os', 'compiler', 'build_type', 'arch'

    def set_version(self):
        git = Git(self.recipe_folder)
        tag_version = get_tag_version(git)

        if tag_version is not None:
            self.version = tag_version
        else:
            self.version = '{}-{}'.format(git.get_branch(), git.get_commit())

        self.custom_cmake_defs.update({
            'TINYREFL_GIT_BRANCH': git.get_branch(),
            'TINYREFL_GIT_COMMIT': git.get_commit(),
            'TINYREFL_GIT_AT_TAG': (tag_version is not None),
            'TINYREFL_LAST_TAG': git.get_last_tag()
        })

    def package_info(self):
        super().package_info()

        if self.settings.compiler in ['gcc', 'clang']:
            self.cpp_info.cxxflags.append('-Wno-gnu-string-literal-operator-template')

