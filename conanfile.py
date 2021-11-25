from conans import ConanFile, tools, Meson
import os

class ConanFileToolsTest(ConanFile):
    generators = "pkg_config"
    requires = ["cppzmq/4.8.1", "openssl/3.0.0", "libcurl/7.79.1"]
    settings = "os", "compiler", "build_type"

    def build(self):
        meson = Meson(self)
        meson.configure(source_folder="%s/src" % self.source_folder,
                        build_folder="build")
        meson.build()