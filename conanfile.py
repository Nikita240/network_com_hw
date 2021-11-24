from conans import ConanFile, tools, Meson
import os

class ConanFileToolsTest(ConanFile):
    generators = "pkg_config"
    requires = "cppzmq/4.8.1"
    settings = "os", "compiler", "build_type"

    def build(self):
        meson = Meson(self)
        meson.configure(build_folder="build", source_folder="src")
        meson.build()