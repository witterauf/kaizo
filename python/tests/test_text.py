import pytest
import kaizo.text as txt

class TestAsciiEncoding:
    def test_decode(self):
        offset, text = txt.AsciiEncoding.decode(b'test', 0)
        assert offset == 4
        assert text == 'test'

    def test_encode(self):
        assert bytes(txt.AsciiEncoding.encode('test')) == b'test\x00'

class TestTableEncoding:
    def test_encode(self):
        entries = [txt.TableEndEntry('end')]
        entries += [txt.TableTextEntry(c) for c in ['a', 'b', 'c']]
        binary = [n.to_bytes(1, byteorder='little') for n in range(len(entries))]
        entries = list(zip(binary, entries))
        table = txt.Table(entries=entries)
        encoding = txt.TableEncoding(table)
        assert bytes(encoding.encode('abc{end}')) == bytes([1, 2, 3, 0])
