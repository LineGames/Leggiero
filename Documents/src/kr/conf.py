# Configuration file for the Sphinx documentation builder.

import sphinx_rtd_theme


# -- Project information -----------------------------------------------------

project = 'Leggiero'
copyright = '2021, Kim, Young Soo'
author = 'Kim, Young Soo'


# -- General configuration ---------------------------------------------------

extensions = [
    'sphinx_rtd_theme'
]

templates_path = []

language = 'ko'

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

html_theme_options = {
    'navigation_depth': 2, 
    'collapse_navigation': False,
}

html_static_path = []
