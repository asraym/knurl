"""pkg package init. Docstring guard check: from x import y should not be parsed here."""
from . import helper       # 1 dot = pkg itself -> pkg.helper
from .. import main        # 2 dots from "pkg" strips to root "" -> resolves "main" at root (valid, not external)
