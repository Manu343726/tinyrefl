from conans.model.conan_file import ConanFile
from conans import CMake
import os

class TinyreflTestPackage(ConanFile):
    generators = 'cmake', 'cmake_find_package'

    def build_requirements(self):
        # TODO: Get version from deps_cpp_info (Seems empty)
        self.build_requires('tinyrefl-tool/{}'.format('0.5.2'))

    def build(self):
        cmake = CMake(self, parallel=False)
        cmake.verbose = True
        cmake.configure()
        cmake.build(target='clean-tinyrefl')
        cmake.build()

    def test(self):
        self.run(os.path.join('bin', 'tinyrefl_example'))
