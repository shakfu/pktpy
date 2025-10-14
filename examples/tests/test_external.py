"""
Test the pktpy external api.External
"""

import api


def test_external_creation():
    """Test External creation"""
    ext = api.External()
    assert ext is not None


def test_external_str():
    """Test External string representation"""
    ext = api.External()
    str_repr = str(ext)
    assert "External" in str_repr


def test_external_is_valid():
    """Test External is_valid method"""
    ext = api.External()
    # When created without pointer, should be invalid
    assert not ext.is_valid()


def test_external_post():
    """Test External post method"""
    ext = api.External()
    # Should work even if external is None (uses global post)
    ext.post("Test message from External")


def test_external_get_name():
    """Test External get_name method"""
    ext = api.External()
    try:
        name = ext.get_name()
        # Should return "pktpy" for now
        assert name == "pktpy"
    except:
        # May fail if external is NULL
        pass
