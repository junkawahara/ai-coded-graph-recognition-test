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

# -- Options for HTML output -----------------------------------------------
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'navigation_depth': 3,
    'collapse_navigation': False,
}
html_static_path = []
