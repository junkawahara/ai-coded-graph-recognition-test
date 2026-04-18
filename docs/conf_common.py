# Common Sphinx configuration shared across all language builds.

import os
import subprocess

# -- Run Doxygen (once, from the docs root) --------------------------------
_docs_root = os.path.dirname(__file__)
subprocess.call('doxygen Doxyfile', shell=True, cwd=_docs_root)

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
    'graph_recognition': os.path.join(_docs_root, '_doxygen', 'xml'),
}
breathe_default_project = 'graph_recognition'
breathe_default_members = ('members', 'undoc-members')

templates_path = []
exclude_patterns = ['_build', '_doxygen']

# -- Suppress noise from external / standard-library symbols ---------------
nitpick_ignore = [
    # Our own namespace: Breathe emits scope references that Sphinx's C++
    # domain cannot satisfy, since nothing explicitly declares the namespace.
    ('cpp:identifier', 'graph_recognition'),
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

# -- Options for HTML output -----------------------------------------------
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'navigation_depth': 3,
    'collapse_navigation': False,
}
html_static_path = []
