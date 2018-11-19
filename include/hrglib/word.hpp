#pragma once
#include "hrglib/node.hpp"
#include "hrglib/token.hpp"
#include "hrglib/syllable.hpp"

namespace hrglib {

class word: public node_<R::Word> {
    using base = node_<R::Word>;
    using base::base;
    friend node::default_factory;
};

}
