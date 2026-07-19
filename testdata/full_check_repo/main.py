"""
Module docstring containing a fake import to test the docstring guard.
from nowhere import nothing
import fake_module
"""
import os
import pkg
from pkg.sub import deep      # dotted, non-empty syntax -> resolves to pkg.sub only, deep discarded
import requests as req        # aliasing, stripped
from pkg import (
    helper,
    utils,
)                              # multi-line dotted paren: ONE edge to pkg, continuation lines discarded
from . import cyclic3          # pure relative import at root: 1 dot = root package "", resolves "cyclic3"

print(os.getcwd())
