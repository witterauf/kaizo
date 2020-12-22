class CharacterGrid:
    @staticmethod
    def load_file(self, filename):
        with open(filename, 'r') as f:
            lines = f.readlines()
            width = max(lines, key=len)
            return CharacterGrid(lines, width)

    def __init__(self, contents, width):
        self.contents = contents
        self.width = width
        self.height = len(contents)
    
    def get_character(self, row, col):
        if len(self.contents[row]) <= col < self.width:
            return ' '
        else:
            return self.contents[row][col]

    def characters(self):
        for row in range(self.height):
            for col in range(self.width):
                yield self.get_character(row, col)

    def __len__(self):
        return self.width * self.height
