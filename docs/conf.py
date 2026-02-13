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
exclude_patterns = ['_build', '_doxygen']

language = 'ja'

# -- Options for HTML output -----------------------------------------------
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'navigation_depth': 3,
    'collapse_navigation': False,
}
html_static_path = []
