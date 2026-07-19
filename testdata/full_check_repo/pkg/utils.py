from . import helper    # closes cycle: helper <-> utils
import sys

def compute():
    return sys.path      # Tier2: sys only used inside a function body, not top-level
