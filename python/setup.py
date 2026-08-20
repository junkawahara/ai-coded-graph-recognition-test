import os
import shutil

from setuptools import setup, find_packages
from setuptools.command.sdist import sdist as _sdist
from pybind11.setup_helpers import Pybind11Extension, build_ext

here = os.path.dirname(os.path.abspath(__file__))

# The extension compiles against the headers of the parent repository when
# built from a checkout. An sdist instead ships a copy of the headers under
# ./include (see SdistWithHeaders below), so standalone sdists build too.
repo_include = os.path.join(here, os.pardir, "include")
bundled_include = os.path.join(here, "include")
if os.path.isdir(repo_include):
    include_dir = repo_include
elif os.path.isdir(bundled_include):
    include_dir = bundled_include
else:
    raise RuntimeError(
        "C++ headers not found at {} or {}; graph-recognition must be "
        "built from a repository checkout or from an sdist created by "
        "'python -m build --sdist' (which bundles the headers)".format(
            repo_include, bundled_include
        )
    )


class SdistWithHeaders(_sdist):
    """sdist that copies the parent repository's include/ into the archive."""

    def make_release_tree(self, base_dir, files):
        _sdist.make_release_tree(self, base_dir, files)
        target = os.path.join(base_dir, "include")
        if os.path.isdir(target):
            shutil.rmtree(target)
        shutil.copytree(include_dir, target)


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
    cmdclass={"build_ext": build_ext, "sdist": SdistWithHeaders},
)
