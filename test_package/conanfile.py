from conans.model.conan_file import ConanFile
from conans import CMake
import os

class DefaultNameConan(ConanFile):
    name = "DefaultName"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    build_requires = 'tinyrefl-tool/0.3.1@Manu343726/testing'

    def build(self):
        cmake = CMake(self, parallel=False)
        cmake.verbose = True
        cmake.configure()
        cmake.build()

    def test(self):
        self.run(os.path.join('bin', 'tinyrefl_example'))
