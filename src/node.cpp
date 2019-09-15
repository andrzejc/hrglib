#include "hrglib/node.hpp"
#include "hrglib/features.hpp"
#include "hrglib/error.hpp"
#include "hrglib/map_find.hpp"
#include "hrglib/relation.hpp"
#include "hrglib/graph.hpp"
#include "hrglib/token.hpp"
#include "hrglib/word.hpp"
#include "hrglib/syllable.hpp"
#include "hrglib/optional.hpp"

#include <typeinfo>
#include <utility>
#include <unordered_map>
#include <cassert>
#include <stdexcept>

namespace hrglib {

void detail::throw_bad_dereference(const std::type_info& type) {
    throw error::bad_dereference{type};;
}

relation_name node::relation_name() const noexcept {
    return relation().name();
}

graph& node::graph() noexcept {
    return relation().graph();
}

const node::relation_validator_type& node::relation_validator() const noexcept {
    return graph().relation_validator();
}

struct node::contents {
    hrglib::features features;
    node_map relations;
};

node::contents& node::attach_contents(const hrglib::relation& r, node* in_other_relation) {
    if (nullptr != in_other_relation) {
        auto& cont = in_other_relation->cont_;
        if (map_find(cont.relations, r.name())) {
            throw error::relation_exists{r.name()};
        }
        cont.relations.emplace(r.name(), std::ref(*this));
        return cont;
    } else {
        return *new contents{{}, {{r.name(), std::ref(*this)}}};
    }
}

void node::release_contents(const hrglib::relation& r) noexcept {
    assert(cont_.relations.size() >= 1);
    if (1 == cont_.relations.size()) {
        // if removing last reference, it has to be this
        assert(this == &cont_.relations.begin()->second.get());
        assert(r.name() == cont_.relations.begin()->first);
        delete &cont_;
    } else {
        assert(map_find(cont_.relations, r.name()));
        assert(this == &map_find(cont_.relations, r.name())->get());
        auto num = cont_.relations.erase(r.name());
        assert(1 == num);
    }
}

const node::node_map& node::relations() const noexcept {
    return cont_.relations;
}

hrglib::features& node::features() noexcept {
    return cont_.features;
}

bool node::in(hrglib::relation_name rel) const noexcept {
    return !!map_find(cont_.relations, rel);
}

bool node::in(const hrglib::relation& rel) const noexcept {
    return in(rel.name());
}

node::navigator node::as(hrglib::relation_name rel) noexcept {
    if (auto res = map_find(cont_.relations, rel)) {
        return {&res->get()};
    } else {
        return {};
    }
}

inline void node::unlink_next() {
    if (next_ != nullptr) {
        assert(this == next_->prev_);
        next_->prev_ = nullptr;
    }
}

inline void node::unlink_prev() {
    if (prev_ != nullptr) {
        assert(this == prev_->next_);
        prev_->next_ = nullptr;
    }
}

inline void node::unlink_parent() {
    if (auto parent = static_cast<node*>(parent_); parent != nullptr) {
        if (this == parent->first_child_) {
            parent->first_child_ = next_;
        }
        if (this == parent->last_child_) {
            parent->last_child_ = prev_;
        }
    }
}

inline void node::unlink_children() {
    if (first_child_ != nullptr) {
        for (auto child = first_child_; child != nullptr; child = child->next_) {
            assert(this == child->parent_);
            child->parent_ = nullptr;
            if (last_child_ == child) {
                // don't go beyond last_child if relation closed
                break;
            }
        }
    }
}

node::~node() noexcept {
    unlink_children();
    unlink_parent();
    unlink_prev();
    unlink_next();
    release_contents(rel_);
}

node& node::set_next_(node* n) {
    if (next_ == n) {
        return *this;
    }
    if (nullptr != n && &relation() != &n->relation()) {
        throw error::bad_relation{n->relation().name()};
    }
    // break the link in both direction
    unlink_next();
    // set new next
    if ((next_ = n) != nullptr) {
        // break also old link from new next
        n->unlink_prev();
        // setup link in both directions
        next_->prev_ = this;
    }
    return *this;
}

node& node::set_next(node* n) {
    return set_next_(n);
}

node& node::set_prev_(node* n) {
    if (prev_ == n) {
        return *this;
    }
    if (nullptr != n && &relation() != &n->relation()) {
        throw error::bad_relation{n->relation().name()};
    }
    unlink_prev();
    if ((prev_ = n) != nullptr) {
        n->unlink_next();
        prev_->next_ = this;
    }
    return *this;
}

node& node::set_prev(node* n) {
    return set_prev_(n);
}

node& node::set_parent_(node* p) {
    if (parent_ == p) {
        return *this;
    }
    if (nullptr != p && &graph() != &p->graph()) {
        throw std::invalid_argument{"linked nodes must be in the same graph"};
    }
    unlink_parent();
    parent_ = p;
    return *this;
}

node& node::set_parent(node* p) {
    if (nullptr != p && !relation_validator()(p->relation(), relation())) {
        throw error::bad_relation{p->relation().name()};
    }
    return set_parent_(p);
}

namespace {
inline node& node_set_child(node& self, node*& field, node* c) {
    if (field == c) {
        return self;
    }
    if (nullptr != c && &self.graph() != &c->graph()) {
        throw std::invalid_argument{"linked nodes must be in the same graph"};
    }
    if ((field = c) != nullptr) {
        c->set_parent(&self);
    }
    return self;
}
}

node& node::set_first_child_(node* c) {
    return node_set_child(*this, first_child_, c);
}

node& node::set_last_child_(node* c) {
    return node_set_child(*this, last_child_, c);
}

node& node::set_first_child(node* c) {
    if (nullptr != c && !relation_validator()(relation(), c->relation())) {
        throw error::bad_relation{c->relation().name()};
    }
    return set_first_child_(c);
}

node& node::set_last_child(node* c) {
    if (nullptr != c && !relation_validator()(relation(), c->relation())) {
        throw error::bad_relation{c->relation().name()};
    }
    return set_last_child_(c);
}

unique_ptr<node> node::default_factory::operator()(rel_t& r, node* c) const {
    switch (r.name()) {
#define HANDLE_CASE(rel) \
    case R:: rel : \
        return unique_ptr<node>{new node_t<R:: rel>{r, c}};

    // TODO handle all default relations
    // HRGLIB_RELATION_LIST(HANDLE_CASE)
    HANDLE_CASE(Token)
    HANDLE_CASE(Word)
    HANDLE_CASE(Syllable)
#undef HANDLE_CASE
    default:
        throw error::bad_relation{r.name()};
    }
}

bool node::default_relation_validator::operator()(const hrglib::relation& parent, const hrglib::relation& child) const {
#define CHECK_RELATION_TRAIS(relation) \
    if (child.name() == R:: relation && parent_relation<R:: relation> == parent.name() || \
        parent.name() == R:: relation && child_relation<R:: relation> == child.name()) \
    { \
        return true; \
    }
    // TODO handle all default relations
    // HRGLIB_RELATION_LIST(CHECK_RELATION_TRAIS)
    CHECK_RELATION_TRAIS(Token)
    CHECK_RELATION_TRAIS(Word)
    CHECK_RELATION_TRAIS(Syllable)
#undef CHECK_RELATION_TRAIS
    return false;
}

node& node::insert_next(node* in_other_relation) {
    auto& res = rel_.create(in_other_relation);
    set_next_(&res.set_next_(next_));
    if (rel_.last() == this) {
        rel_.set_last(&res);
    }
    return res;
}

node& node::insert_prev(node* in_other_relation) {
    auto& res = rel_.create(in_other_relation);
    set_prev_(&res.set_prev_(prev_));
    if (rel_.first() == this) {
        rel_.set_first(&res);
    }
    return res;
}
}  // namespace hrglib
