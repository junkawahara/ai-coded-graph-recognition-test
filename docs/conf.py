# Japanese documentation configuration (built from the docs root).

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from conf_common import *  # noqa: F401,F403

language = 'ja'

# The Japanese build runs from the docs root itself, so keep the English
# tree out of it.
exclude_patterns = exclude_patterns + ['en']
