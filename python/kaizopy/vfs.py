def vfsiter(vfs, level=0):
    for index in range(vfs.file_count):
        yield (vfs, index, level)
        if vfs.is_folder(index):
            next_vfs = vfs.open_folder(index)
            yield from vfsiter(next_vfs, level + 1)
