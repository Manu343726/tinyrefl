from conans.model.conan_file import ConanFile
from conans import CMake
import os

class TinyreflTestPackage(ConanFile):
    generators = 'cmake'

    def build_requirements(self):
        tinyrefl_version = self.requires['tinyrefl'].ref.version
        self.build_requires('tinyrefl-tool/{}'.format(tinyrefl_version))

    def build(self):
        cmake = CMake(self, parallel=False)
        cmake.verbose = True
        cmake.configure()
        cmake.build(target='clean-tinyrefl')
        cmake.build()

    def test(self):
        self.run(os.path.join('bin', 'tinyrefl_example'))
