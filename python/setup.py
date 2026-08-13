from setuptools import setup, find_packages
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os

here = os.path.dirname(os.path.abspath(__file__))
include_dir = os.path.join(here, os.pardir, "include")

# The extension compiles against the headers of the parent repository.
# A standalone sdist would not contain them, so fail early with a clear
# message instead of an obscure compiler error.
if not os.path.isdir(include_dir):
    raise RuntimeError(
        "C++ headers not found at {}; graph-recognition must be built "
        "from a full repository checkout (standalone sdists are not "
        "supported)".format(include_dir)
    )

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
