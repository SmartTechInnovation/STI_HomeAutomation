#include "page.h"

Page_class::Page_class() {

}

void Page_class::addBlockInstance(BlockBase_class *BlockRef){
    m_vec_Blocks.push_back(BlockRef);
}