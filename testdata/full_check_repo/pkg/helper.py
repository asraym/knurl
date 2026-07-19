import pkg.utils as u          # dotted absolute, aliased -> resolves to pkg.utils
from .utils import compute     # dotted relative, symbol discarded -> resolves to pkg.utils (same target again)
import unused_stdlib_stub      # Tier3 candidate: imported, never referenced again

print(u)  # top-level use -> Tier1 for the pkg.utils edge via 'u' binding
