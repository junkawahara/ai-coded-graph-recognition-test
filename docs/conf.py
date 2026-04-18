# Configuration file for the Sphinx documentation builder.

import subprocess
import os

# -- Run Doxygen -----------------------------------------------------------
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
subprocess.call('doxygen Doxyfile', shell=True, cwd=os.path.dirname(__file__))

# -- Project information ---------------------------------------------------
project = 'Graph Recognition Library'
copyright = '2025'
author = ''

# -- General configuration -------------------------------------------------
extensions = [
    'breathe',
    'sphinx.ext.autodoc',
]

breathe_projects = {
    'graph_recognition': os.path.join(os.path.dirname(__file__), '_doxygen', 'xml'),
}
breathe_default_project = 'graph_recognition'
breathe_default_members = ('members', 'undoc-members')

templates_path = ['_templates']
exclude_patterns = ['_build', '_doxygen', 'en']

# -- Suppress noise from external / standard-library symbols ---------------
nitpick_ignore = [
    # Our own namespace: Breathe emits scope references that Sphinx's C++
    # domain cannot satisfy, since nothing explicitly declares the namespace.
    ('cpp:identifier', 'graph_recognition'),
    # The Japanese build does not yet ship per-family pages, so the clique
    # helpers in api/utilities.rst cannot resolve their ChordalResult argument
    # nor the @see check_chordal() cross-reference that Doxygen emits for
    # CliqueTreeResult.
    ('cpp:identifier', 'ChordalResult'),
]
nitpick_ignore_regex = [
    # Breathe emits Doxygen anchor IDs like
    # "namespacegraph__recognition_1a<hex>" that point to functions
    # documented only on family pages. Skip them instead of duplicating the
    # declarations here.
    ('std:ref', r'namespacegraph__recognition_1a[0-9a-f]+'),
    ('cpp:identifier', 'size_t'),
    ('cpp:identifier', 'std'),
    ('cpp:identifier', 'std::size_t'),
    ('cpp:identifier', 'std::vector'),
    ('cpp:identifier', 'std::string'),
    ('cpp:identifier', 'std::pair'),
    ('cpp:identifier', 'std::map'),
    ('cpp:identifier', 'std::set'),
    ('cpp:identifier', 'std::unordered_map'),
    ('cpp:identifier', 'std::unordered_set'),
    ('cpp:identifier', 'std::queue'),
    ('cpp:identifier', 'std::list'),
    ('cpp:identifier', 'std::function'),
    ('cpp:identifier', 'std::ostream'),
    ('cpp:identifier', 'std::istream'),
    ('cpp:identifier', 'std::tuple'),
    ('cpp:identifier', 'std::array'),
    ('cpp:identifier', 'std::deque'),
    ('cpp:identifier', 'T'),
    ('cpp:identifier', 'uint8_t'),
    ('cpp:identifier', 'uint32_t'),
    ('cpp:identifier', 'uint64_t'),
    ('cpp:identifier', 'int32_t'),
    ('cpp:identifier', 'int64_t'),
]

language = 'ja'

# -- Options for HTML output -----------------------------------------------
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'navigation_depth': 3,
    'collapse_navigation': False,
}
html_static_path = []
