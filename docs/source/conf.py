# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os

from clang.cindex import Config
from hawkmoth.util import compiler
from sphinx.ext import napoleon

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'mat47'
copyright = '2022, AnonymouX47'
author = 'AnonymouX47'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.napoleon",
    "hawkmoth",
]

templates_path = []
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_static_path = []

Config.set_library_path("/usr/lib")
Config.set_library_file("libclang.so")

cautodoc_root = os.path.abspath("../../src/mat47")
cautodoc_clang = compiler.get_include_args("gcc")
cautodoc_transformations = {None: lambda s: str(napoleon.docstring.GoogleDocstring(s))}
