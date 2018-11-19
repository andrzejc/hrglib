#pragma once
#include "hrglib/node.hpp"
#include "hrglib/word.hpp"

namespace hrglib {

class token: public node_<R::Token> {
    using base = node_<R::Token>;
    using base::base;
    friend node::default_factory;
};

}
