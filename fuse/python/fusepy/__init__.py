from abc import ABC, abstractmethod
from fusepy._fusepy import *
from fusepy._fusepy import _TextEncoding
from fusepy._fusepy import _AsciiEncoding
from fusepy.format import *
from fusepy.binary import *
from fusepy.objects import *
from fusepy.data import read_hierarchical_csv
from fusepy._fusepy import Reference
import fusepy.addresses
from fusepy.linking import *

class TextEncoding(ABC):

    @property
    @abstractmethod
    def supports_encoding(self):
        pass

    @property
    @abstractmethod
    def supports_decoding(self):
        pass

    @abstractmethod
    def encode(self, text):
        pass

    @abstractmethod
    def decode(self, buffer, offset):
        pass

class ExtensionTextEncoding(TextEncoding):
    def __init__(self, encoding):
        if not issubclass(type(encoding), _TextEncoding):
            raise TypeError("expected a _TextEncoding")
        self._encoding = encoding

    @property
    def supports_encoding(self):
        return self._encoding.supports_encoding

    @property
    def supports_decoding(self):
        return self._encoding.supports_decoding

    def encode(self, text):
        return self._encoding.encode(text)

    def decode(self, buffer, offset):
        return self._encoding.decode(buffer, offset)

AsciiEncoding = ExtensionTextEncoding(_AsciiEncoding)