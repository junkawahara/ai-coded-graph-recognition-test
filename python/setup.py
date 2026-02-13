from setuptools import setup, find_packages
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os

here = os.path.dirname(os.path.abspath(__file__))
include_dir = os.path.join(here, os.pardir, "include")

ext_modules = [
    Pybind11Extension(
        "graph_recognition._core",
        [os.path.join("src", "graph_recognition", "_bindings.cpp")],
        include_dirs=[include_dir],
        cxx_std=11,
    ),
]

setup(
    name="graph-recognition",
    version="0.1.0",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
