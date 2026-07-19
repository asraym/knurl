from .... import nonexistent    # 4 dots from "pkg.sub" (2 segments): strip1->pkg, strip2->"", strip3->past root -> nullopt -> external
from ... import also_root_hit   # 3 dots from "pkg.sub": strip1->pkg, strip2->"" -> lands exactly on root (VALID, not external) -> resolves "also_root_hit" if present, else external for a different reason (name not found, not overflow)
from ..helper import unused_symbol   # 2 dots from "pkg.sub" -> "pkg", + "helper" -> resolves to pkg.helper
import os
