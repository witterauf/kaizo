import pytest
import kaizo.addresses as adr

class TestAbsoluteAddressFormat:
    def test_from_int(self):
        assert str(adr.FileOffset.from_int(1)) == "0x00000001"