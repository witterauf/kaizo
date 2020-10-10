from abc import ABC, abstractmethod
from kaizopy._kaizopy import _VirtualFileSystem
import re

def vfsiter(vfs, level=0):
    for index in range(vfs.file_count):
        yield (vfs, index, level)
        if vfs.is_folder(index):
            next_vfs = vfs.open_folder(index)
            yield from vfsiter(next_vfs, level + 1)

class VfsPath:

    def __init__(self, path):
        self._parts = path.split('/')
        self._parts = list(filter(None, self._parts))

    @property
    def parts(self):
        return self._parts

    def __repr__(self):
        return f"VfsPath({str(self)})"

    def __str__(self):
        return "/".join(self._parts)

class VirtualFileSystem(ABC):
    @property
    @abstractmethod
    def file_count(self):
        pass

    @abstractmethod
    def is_folder_by_index(self, index):
        pass

    @abstractmethod
    def file_size_by_index(self, index):
        pass

    @abstractmethod
    def file_name(self, index):
        pass

    @abstractmethod
    def file_index(self, name):
        pass

    @abstractmethod
    def open_folder_by_index(self, index):
        pass

    @abstractmethod
    def open_file_by_index(self, index):
        pass

    def _name_to_index(self, name, func):
        index = self.file_index(name)
        if index:
            return func(index)
        else:
            raise ValueError(f"file {name} does not exist")

    def is_folder(self, name):
        return self._name_to_index(name, self.is_folder_by_index)

    def open_folder(self, name):
        return self._name_to_index(name, self.open_folder_by_index)

    def glob(self, pattern):
        pattern_parts = pattern.split('/')
        pattern_parts  = list(filter(None, pattern_parts))
        return self._glob(pattern_parts)

    def _glob(self, pattern_list):
        if not pattern_list:
            return []
        pattern = pattern_list[0]
        pattern = pattern.replace("*", ".*")
        files = []
        for i in range(self.file_count):
            name = self.file_name(i)
            if re.match(pattern, name):
                if self.is_folder_by_index(i) and len(pattern_list) > 1:
                    folder = self.open_folder_by_index(i)
                    folder_files = folder._glob(pattern_list[1:])
                    for file_name in folder_files:
                        files.append(name + "/" + file_name)
                else:
                    files.append(name)
        return files

class NativeVirtualFileSystem(VirtualFileSystem):

    def __init__(self, vfs):
        if not issubclass(type(vfs), _VirtualFileSystem):
            raise TypeError("must be a subclass of _VirtualFileSystem")
        self._vfs = vfs

    @property
    def file_count(self):
        return self._vfs.file_count

    def is_folder_by_index(self, index):
        return self._vfs.is_folder(index)

    def file_size_by_index(self, index):
        return self._vfs.file_size(index)

    def file_index(self, name):
        return self._vfs.file_index(name)

    def file_name(self, index):
        return self._vfs.file_name(index)

    def open_folder_by_index(self, index):
        sub_vfs = self._vfs.open_folder(index)
        if issubclass(type(sub_vfs), _VirtualFileSystem):
            return NativeVirtualFileSystem(sub_vfs)
        else:
            return sub_vfs

    def open_file_by_index(self, index):
        return self._vfs.open_file(index)