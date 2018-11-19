#pragma once
#include "hrglib/node.hpp"
#include "hrglib/word.hpp"

namespace hrglib {

class syllable: public node_<R::Syllable> {
    using base = node_<R::Syllable>;
    using base::base;
    friend node::default_factory;
};

}
