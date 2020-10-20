from fusepy._fusepy import _DataReader

class DataReader:
    def __init__(self, binary):
        self._reader = _DataReader(binary)

    def set_offset(self, offset):
        self._reader.set_offset(offset)

    def set_address_map(self, map):
        self._reader.set_address_map(map._map)