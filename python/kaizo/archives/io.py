from pathlib import Path
import io

class ConstrainedReader:
    """Constrains reading to a sub-region of an original file/reader object"""

    def __init__(self, source, offset, size):
        if not source.readable():
            raise TypeError('given source does not support reading')
        self._source = source
        self._offset = offset
        self._size = size
        self._current_offset = 0

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def seek(self, offset, whence=0):
        # Only update internal offset; actual seeking is done before reading.
        if whence == 0:            
            self._current_offset = offset
        elif whence == 1:
            self._current_offset = self._current_offset + offset
        elif whence == 2:
            self._current_offset = self._size + offset
        self._current_offset = max(0, self._current_offset)
        self._current_offset = min(self._size, self._current_offset)

    def tell(self):
        return self._current_offset

    def read(self, size=-1):
        if size == -1 or self._current_offset + size >= self._size:
            size = self._size - self._current_offset
        # Underlying reader might have been mutated, so make sure to be at the correct offset.        
        self._source.seek(self._offset + self._current_offset)
        data = self._source.read(size)
        self._current_offset += len(data)
        return data

    def close(self):
        pass

    def write(self, binary):
        raise OSError('SubReader does not support writing')

    def readable(self):
        return True

    def writable(self):
        return False

    def seekable(self):
        return True

def copy(source, dest, size, chunk_size=16*1024*1024, observer=None):
    chunks = int(size // chunk_size)
    for chunk in range(chunks):
        data = source.read(chunk_size)
        dest.write(data)
        if observer is not None:
            observer(chunk * chunk_size, size)
    if size % chunk_size != 0:
        data = source.read(size % chunk_size)
        dest.write(data)
    if observer is not None:
        observer(size, size)

def _replace_files(source, destination, replacements, superfluous_ok=False):
    covered = set(replacements)
    for i in range(source.member_count):
        dest = destination.create_file()
        replacement = None
        if i in replacements:
            replacement = replacements[i]
            covered.remove(i)
        elif source.path(i) in replacements:
            replacement = replacements[source.path(i)]
            covered.remove(source.path(i))
        if replacement is None:
            with source.open(i) as src:
                copy(src, dest, source.member(i).size)
        elif isinstance(replacement, Path):
            with replacement.open('rb') as src:
                copy(src, dest, replacement.stat().st_size)
        elif isinstance(replacement, bytes) or isinstance(replacement, bytearray):
            dest.write(replacement)
        else:
            current = replacement.tell()
            replacement.seek(0, io.SEEK_END)
            size = replacement.tell()
            replacement.seek(current, io.SEEK_SET)
            copy(replacement, dest, size)
        destination.finish_file()
    destination.finish()

    if not superfluous_ok and covered:
        raise ValueError(f'_replace_files: there were {len(covered)} uncovered replacements: ' + ', '.join(covered))