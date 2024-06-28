from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

class Image2AsciiRecipe(ConanFile):
    name = "img2ascii"
    version = "1.0"

    # Optional metadata
    license = "MIT"
    author = "Elvis Dukaj"
    url = ""
    description = "A simple program to convert image to ascii art"
    topics = ("image", "ascii-art")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self):
        self.requires("cpr/1.10.5")
        self.requires("cxxopts/3.2.0")
        self.requires("fmt/10.2.1")
        self.requires("stb/cci.20240213")
        self.requires("opencv/4.9.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
