import os
import shutil

from setuptools import setup, find_packages
from setuptools.command.sdist import sdist as _sdist
from pybind11.setup_helpers import Pybind11Extension, build_ext

here = os.path.dirname(os.path.abspath(__file__))

# The extension compiles against the headers of the parent repository when
# built from a checkout. An sdist instead ships a copy of the headers under
# ./include (see SdistWithHeaders below), so standalone sdists build too.
#
# A bare isdir() test is not enough: unpacking the sdist inside any
# directory that happens to have its own include/ (a venv root, a conda
# prefix, an unrelated C++ checkout) must not shadow the bundled headers.
# Require the sentinel header graph.h, and prefer the bundled copy -- in a
# repository checkout ./include does not exist, so the parent headers are
# used there as before.
def _has_headers(candidate):
    return os.path.isfile(os.path.join(candidate, "graph.h"))


bundled_include = os.path.join(here, "include")
repo_include = os.path.join(here, os.pardir, "include")
if _has_headers(bundled_include):
    include_dir = bundled_include
elif _has_headers(repo_include):
    include_dir = repo_include
else:
    raise RuntimeError(
        "C++ headers (graph.h) not found at {} or {}; graph-recognition "
        "must be built from a repository checkout or from an sdist created "
        "by 'python -m build --sdist' (which bundles the headers)".format(
            bundled_include, repo_include
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
