/**
 * @file hrglib/graph.hpp
 * @brief Definition of `hrglib::graph` class.
 */
#pragma once
#include "hrglib/node.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/relation_traits.hpp"
#include "hrglib/relation.hpp"
#include "hrglib/memory.hpp"
#include "hrglib/optional.hpp"
#include "hrglib/map_find.hpp"
#include "hrglib/type_traits.hpp"
#include "hrglib/features.hpp"

#ifdef HRGLIB_YAMLCPP_PUBLIC
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/emitter.h>
#endif

#include <unordered_map>
#include <iosfwd>

namespace hrglib {
//! @brief A collection of relations and linked nodes in them.
class graph: private std::unordered_map<relation_name, unique_ptr<relation>> {
    using base = std::unordered_map<relation_name, unique_ptr<relation>>;
    const node::factory_type node_factory_;
    const node::relation_validator_type relation_validator_;
    const relation::factory_type relation_factory_;
    const relation::name_mapper_type relation_name_mapper_;

    template<typename Relation = relation, class Utterance>
    static optional<copy_const_t<Utterance, Relation>&> get_(Utterance& u, relation_name rel) noexcept {
        if (auto ref = map_find<copy_const_t<Utterance, base>>(u, rel)) {
            return {static_cast<copy_const_t<Utterance, Relation>&>(*ref->get())};
        } else {
            return {};
        }
    }

public:
    explicit graph(
            node::factory_type node_factory = nullptr,
            node::relation_validator_type relation_validator = nullptr,
            relation::factory_type relation_factory = nullptr,
            relation::name_mapper_type relation_name_mapper = nullptr
    ):
        node_factory_{ node_factory
            ? std::move(node_factory)
            : node::default_factory{}
        },
        relation_validator_{ relation_validator
            ? std::move(relation_validator)
            : node::default_relation_validator{}
        },
        relation_factory_{ relation_factory
            ? std::move(relation_factory)
            : relation::default_factory{}
        },
        relation_name_mapper_{ relation_name_mapper
            ? std::move(relation_name_mapper)
            : relation::DEFAULT_NAME_MAPPER
        }
    {}

    struct builder {
        node::factory_type node_factory = nullptr;
        node::relation_validator_type relation_validator = nullptr;
        relation::factory_type relation_factory = nullptr;
        relation::name_mapper_type relation_name_mapper = nullptr;

        builder& with_node_factory(node::factory_type nf) {
            node_factory = std::move(nf);
            return *this;
        }
        builder& with_relation_validator(node::relation_validator_type rv) {
            relation_validator = std::move(rv);
            return *this;
        }
        builder& with_relation_factory(relation::factory_type rf) {
            relation_factory = std::move(rf);
            return *this;
        }
        builder& with_relation_name_mapper(relation::name_mapper_type rnm) {
            relation_name_mapper = std::move(rnm);
            return *this;
        }

        graph build() const & {
            return graph{
                node_factory,
                relation_validator,
                relation_factory,
                relation_name_mapper
            };
        }

        graph build() && {
            return graph{
                std::move(node_factory),
                std::move(relation_validator),
                std::move(relation_factory),
                std::move(relation_name_mapper)
            };
        }
    };

    bool has(relation_name rel) const noexcept { return !!get_(*this, rel); }

    optional<const relation&> get(relation_name rel) const noexcept { return get_(*this, rel); }
    optional<relation&> get(relation_name rel) noexcept { return get_(*this, rel); }
    template<relation_name rel>
    optional<const relation_t<rel>&> get() const noexcept {
        return get_<relation_t<rel>>(*this, rel);
    }
    template<relation_name rel>
    optional<relation_t<rel>&> get() noexcept {
        return get_<relation_t<rel>>(*this, rel);
    }

    // This is insert-or-access operation, will create appropriate relation if not exists;
    // graph must be mutable for this.
    relation& at(relation_name rel);
    relation& operator[](relation_name rel) { return at(rel); }
    template<relation_name rel>
    relation_t<rel>& at() {
        return static_cast<relation_t<rel>&>(at(rel));
    }
    template<typename RelationType>
    RelationType& at() {
        return static_cast<RelationType&>(at(RelationType::NAME));
    }

    constexpr const node::factory_type& node_factory() const noexcept { return node_factory_; }
    constexpr const node::relation_validator_type& relation_validator() const noexcept { return relation_validator_; }
    constexpr const relation::factory_type& relation_factory() const noexcept { return relation_factory_; }
    constexpr const relation::name_mapper_type& relation_name_mapper() const noexcept { return relation_name_mapper_; }

    builder to_builder() const {
        return builder{}
            .with_node_factory(node_factory())
            .with_relation_factory(relation_factory())
            .with_relation_name_mapper(relation_name_mapper())
            .with_relation_validator(relation_validator());
    }

    static graph from_file(string_view path, optional<builder> b = nullopt);
    static graph from_stream(std::istream& is, optional<builder> b = nullopt);
    static graph from_string(string_view yaml, optional<builder> b = nullopt);

#ifdef HRGLIB_YAMLCPP_PUBLIC
    static graph from_yaml(const YAML::Node& document, optional<builder> b = nullopt);

    friend YAML::Emitter& operator << (YAML::Emitter& lhs, const graph& rhs);
#endif

    friend std::istream& operator >> (std::istream& lhs, graph& rhs);
    friend std::ostream& operator << (std::ostream& lhs, const graph& rhs);
};
}  // namespace hrglib
