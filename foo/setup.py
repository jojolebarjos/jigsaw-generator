from glob import glob
from setuptools.command.build_ext import build_ext
from setuptools import find_packages, setup, Extension
import numpy as np


CPP17_ARGUMENT = {
    "msvc": "/std:c++17",
    # Known to use default: "clang", "gcc", "mingw32"
}
CPP17_ARGUMENT_DEFAULT = "-std=c++17"

class cpp17_build_ext(build_ext):
    def build_extensions(self):
        compiler = self.compiler.compiler_type
        argument = CPP17_ARGUMENT.get(compiler, CPP17_ARGUMENT_DEFAULT)
        for extension in self.extensions:
            arguments = extension.extra_compile_args or []
            extension.extra_compile_args = [*arguments, argument]
        super().build_extensions()


extensions = [
    Extension(
        name="jigsaw.native",
        language="c++",
        include_dirs=[
            np.get_include(),
            "include",
        ],
        sources=[
            *glob("jigsaw/native/*.cpp"),
        ],
    ),
]


setup(
    name="jigsaw",
    version="0.0.1",
    packages=find_packages(),
    python_requires=">=3.6",
    install_requires=[
        "numpy>1.7",
    ],
    ext_modules=extensions,
    cmdclass={"build_ext": cpp17_build_ext},
)
