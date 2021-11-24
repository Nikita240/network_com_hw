from conans import ConanFile, tools, Meson
import os

class ConanFileToolsTest(ConanFile):
    generators = "pkg_config"
    requires = ["cppzmq/4.8.1", "openssl/3.0.0"]
    settings = "os", "compiler", "build_type"

    def build(self):
        meson = Meson(self)
        meson.configure(source_folder="src")
        meson.build()