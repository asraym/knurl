from pkg import helper   # syntax="pkg" non-empty -> resolves to pkg/__init__.py only (helper discarded, decision #4)
                          # test -> production edge, severity multiplied by testWeight
import os


def test_helper_importable():
    assert helper is not None
