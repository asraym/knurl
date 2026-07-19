import os          # Tier1: used at module top-level (indent 0) below
import sys         # Tier2: only ever used inside a function body
import json        # Tier2 (documented blind spot): only used inside a class body,
                    # which genuinely runs at import time but reads as indented text
import re          # Tier3: imported, never referenced again anywhere

print(os.getcwd())

def foo():
    return sys.version

class Bar:
    # Known documented blind spot: this line genuinely executes at import
    # time, but reads as indented text, so it scores Tier2, not the "true"
    # Tier1 it actually deserves.
    x = json.dumps({})
