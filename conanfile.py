from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, cmake_layout

class TaskFlowConan(ConanFile):
    name = "TaskFlow"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    requires = (
        "boost/1.90.0",
        "spdlog/1.14.1",
        "nlohmann_json/3.12.0",
        "gtest/1.15.0",
        "libpqxx/7.9.2",
        "hiredis/1.2.0",
        "redis-plus-plus/1.3.11",
    )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()
