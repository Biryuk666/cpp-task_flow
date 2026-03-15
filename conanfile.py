from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout

class TaskFlowConan(ConanFile):
    name = "TaskFlow"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    requires = (
        "boost/1.86.0",
        "spdlog/1.14.1",
        "nlohmann_json/3.12.0",
        "gtest/1.15.0",
    )
