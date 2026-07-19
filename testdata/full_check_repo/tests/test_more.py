from . import test_core   # real relative import, tests.test_more -> tests.test_core
from . import conftest    # tests.test_more -> tests.conftest


def test_stub_runs():
    conftest.fixture_stub()
