#include "hrglib/relation.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/error.hpp"
#include "hrglib/graph.hpp"

#include <cassert>
#include <stdexcept>
#include <utility>

namespace hrglib {
unique_ptr<relation> relation::default_factory::operator()(hrglib::graph& g, relation_name rel) const {
    switch (rel) {
#define HANDLE_CASE(rel) \
    case R:: rel : \
        return unique_ptr<relation>{new relation_t<R:: rel> {g}};

    // TODO handle all default relations
    // HRGLIB_RELATION_LIST(HANDLE_CASE)
    HANDLE_CASE(Token)
    HANDLE_CASE(Word)
    HANDLE_CASE(Syllable)
#undef HANDLE_CASE
    default:
        throw error::bad_relation{rel};
    }
}

relation& relation::set_first(node* n) {
    if (nullptr != n && &n->relation() != this) {
        throw error::bad_relation{n->relation_name()};
    }
    return set_first_(n);
}

relation& relation::set_last(node* n) {
    if (nullptr != n && &n->relation() != this) {
        throw error::bad_relation{n->relation_name()};
    }
    return set_last_(n);
}

node& relation::create(node* in_other_relation) {
    return **insert(graph_.node_factory()(*this, in_other_relation)).first;
}

void relation::erase(node& n) {
    assert(this == &n.relation());
    if (this != &n.relation()) {
        throw std::invalid_argument{"node in other relation"};
    }
    if (&n == first_) {
        first_ = n.next();
    }
    if (&n == last_) {
        last_ = n.prev();
    }
    std::unique_ptr<node> tmp{&n};
    try {
        base::erase(tmp);
        tmp.release();
    } catch (...) {
        tmp.release();
        throw;
    }
}

node& relation::append(node* in_other_relation) {
    auto& res = create(in_other_relation);
    if (last_ != nullptr) {
        last_->set_next_(&res);
        last_ = &res;
    } else if (first_ != nullptr) {
        // open relation
        node::last_(*first_).set_next_(&res);
    } else {
        // first connected node
        first_ = (last_ = &res);
    }
    return res;
}

node& relation::prepend(node* in_other_relation) {
    auto& res = create(in_other_relation);
    if (first_ != nullptr) {
        first_->set_prev_(&res);
        first_ = &res;
    } else {
        assert(last_ == nullptr);
        first_ = (last_ = &res);
    }
    return res;
}
}  // namespace hrglib
