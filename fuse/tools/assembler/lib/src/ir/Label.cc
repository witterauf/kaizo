#include <fuse/assembler/ir/Label.h>

namespace fuse::assembler {

Label::Label(BlockElementKind kind)
    : BlockElement{kind}
{
}

} // namespace fuse::assembler
