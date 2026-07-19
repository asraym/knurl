from .. import helper           # 2 dots from "pkg.sub" -> strips to "pkg" -> resolves "helper" -> pkg.helper
from . import (
    deep,
    missing_sibling,
)                                # pure-dot multi-line paren: TWO edges, one per name (deep resolves, missing_sibling -> external)
