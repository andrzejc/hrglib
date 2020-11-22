#include "yaml-cpp_config.hpp"
#include "hrglib/graph.hpp"
#include "hrglib/error.hpp"

#include "yaml-cpp.hpp"
#include "utils.hpp"

#include <unordered_map>
#include <functional>
#include <fstream>

namespace hrglib {
relation& graph::at(relation_name rel) {
    if (auto r = map_find<base>(*this, rel)) {
        return **r;
    } else {
        return *insert({rel, relation_factory()(*this, rel)}).first->second;
    }
}

namespace {
template<typename YamlNode>
YamlNode& ensure_object(YamlNode& node, const string& name) {
    if (!node.IsMap()) {
        throw error::parsing_error{"node " + name + " is not object"};
    }
    return node;
}

enum struct pivot {
    next, prev, parent, first_child, last_child, COUNT,
};

using pivot_setter = decltype(&node::set_next);
using pivot_getter = node::const_navigator (node::*)() const;

const struct {
    const string_view name;
    pivot_setter setter;
    pivot_getter getter;
    } pivots[] = {
#define PE(name) { #name, &node::set_ ## name, &node:: name }
    PE(next),
    PE(prev),
    PE(parent),
    PE(first_child),
    PE(last_child),
};

static_assert(std::size(pivots) == static_cast<std::size_t>(pivot::COUNT), "");

struct arc {
    relation_name this_relation;
    pivot_setter setter;
    pivot_getter getter;
    string target_id;

    template<typename YamlValue>
    static arc from_yaml(const YamlValue& obj, const string& key, relation_name rel) {
        const auto end = std::end(pivots);
        auto it = std::find_if(std::begin(pivots), end,
                [&](const auto& pe) {
                    return pe.name == key;
                });
        if (end == it) {
            throw error::parsing_error{"invalid name of relation pivot"};
        }
        return {rel, it->setter, it->getter, obj.second.template as<string>()};
    }
};

struct node_prototype {
    hrglib::features features;
    std::unordered_map<relation_name, string> relations;
    std::vector<arc> arcs;

    void parse_relations(const YAML::Node& rels, const relation::name_mapper_type& nm) {
        for (auto&& rel_prop: rels) {
            const auto rel_name = rel_prop.first.as<string>();
            auto rel = nm(rel_name);
            for (auto&& arcs_prop: ensure_object(rel_prop.second, rel_name)) {
                const auto key = arcs_prop.first.as<string>();
                if ("id" == key) {
                    relations[rel] = arcs_prop.second.as<string>();
                } else {
                    arcs.emplace_back(arc::from_yaml(arcs_prop, key, rel));
                }
            }
        }
    }
};

struct relation_prototype {
    relation_name name;
    optional<string> first_id;
    optional<string> last_id;
};

void build_graph(graph& g, std::vector<node_prototype> nps, std::vector<relation_prototype> rps) {
    std::unordered_map<string, node*> nodes;
    nodes.reserve(nps.size());

    // first create all the nodes
    for (auto&& np: nps) {
        node* n = nullptr;
        for (auto&& kv: np.relations) {
            n = &g[kv.first].create(n);
            if (!nodes.emplace(kv.second, n).second) {
                throw error::parsing_error{"duplicate node id " + kv.second};
            }
            if (!np.features.empty()) {
                n->features() = std::move(np.features);
            }
        }
    }
    for (auto&& np: nps) {
        for (auto&& arc: np.arcs) {
            if (auto rel_id = map_find(np.relations, arc.this_relation)) {
                if (auto from = map_find(nodes, *rel_id)) {
                    if (auto to = map_find(nodes, arc.target_id)) {
                        (*from->*arc.setter)(*to);
                    } else {
                        throw error::parsing_error{"missing node id " + arc.target_id};
                    }
                } else {
                    throw error::parsing_error{"missing node id " + *rel_id};
                }
            } else {
                throw error::parsing_error{"node relation missing id"};
            }
        }
    }
    for (auto&& rp: rps) {
        auto& r = g[rp.name];
        if (rp.first_id) {
            if (auto n = map_find(nodes, *rp.first_id)) {
                r.set_first(*n);
            } else {
                throw error::parsing_error{"missing node id " + *rp.first_id};
            }
        }
        if (rp.last_id) {
            if (auto n = map_find(nodes, *rp.last_id)) {
                r.set_last(*n);
            } else {
                throw error::parsing_error{"missing node id " + *rp.last_id};
            }
        }
    }
}

}  // namespace

graph graph::from_yaml(const YAML::Node& doc, optional<builder> b) {
    auto g = b.value_or(builder{}).build();
    std::vector<node_prototype> nps;
    std::vector<relation_prototype> rps;
    for (auto&& kv: ensure_object(doc, "document root")) {
        const auto key = kv.first.as<string>();
        if ("nodes" == key) {
            if (!kv.second.IsSequence()) {
                throw error::parsing_error{"nodes is not sequence"};
            }
            for (auto&& node: kv.second) {
                node_prototype np;
                for (auto&& prop: ensure_object(node, "nodes element")) {
                    const auto prop_key = prop.first.as<string>();
                    if ("features" == prop_key) {
                        np.features = features::from_yaml(prop.second, g.feature_name_mapper());
                    } else if ("relations" == prop_key) {
                        np.parse_relations(ensure_object(prop.second, prop_key), g.relation_name_mapper());
                    } else {
                        throw error::parsing_error{"invalid node property " + prop_key};
                    }
                }
                nps.emplace_back(std::move(np));
            }
        } else if ("relations" == key) {
            for (auto&& rel: ensure_object(kv.second, key)) {
                const auto rel_str = rel.first.as<string>();
                const auto rel_name = g.relation_name_mapper()(rel_str);
                optional<string> first, last;
                for (auto&& fl: ensure_object(rel.second, rel_str)) {
                    const auto fl_str = fl.first.as<string>();
                    if ("first" == fl_str) {
                        first = fl.second.as<string>();
                    } else if ("last" == fl_str) {
                        last = fl.second.as<string>();
                    } else {
                        throw error::parsing_error{"invalid relation property " + fl_str};
                    }
                }
                rps.emplace_back(relation_prototype{rel_name, std::move(first), std::move(last)});
            }
        } else {
            throw error::parsing_error{"invalid root property " + key};
        }
    }
    build_graph(g, std::move(nps), std::move(rps));
    return g;
}

graph graph::from_string(string_view yaml, optional<builder> b) {
    return graph::from_yaml(YAML::Load(string{yaml}), std::move(b));
}

graph graph::from_stream(std::istream& is, optional<builder> b) {
    return graph::from_yaml(YAML::Load(is), std::move(b));
}

graph graph::from_file(string_view path, optional<builder> b) {
    std::ifstream fs{string{path}.c_str()};
    return graph::from_stream(fs, std::move(b));
}

YAML::Emitter& operator << (YAML::Emitter& out, const graph& g) {
    std::unordered_map<const node*, string> nodes;
    std::size_t id = 0;
    for (auto&& rel: g) {
        for (auto&& n: std::as_const(*rel.second)) {
            nodes[&n] = std::to_string(id++);
        }
    }
    out << YAML::BeginMap << YAML::Key << "nodes" << YAML::Value << YAML::BeginSeq;
    std::unordered_set<const features*> feats_seen;
    for (auto&& rel: g) {
        for (auto&& nn: std::as_const<relation::base>(*rel.second)) {
            auto& n = *nn;
            if (feats_seen.find(&n.features()) != feats_seen.end()) {
                continue;
            }
            feats_seen.insert(&n.features());
            out << YAML::BeginMap << YAML::Key << "features" << YAML::Value << n.features();
            out << YAML::Key << "relations" << YAML::Value << YAML::BeginMap;
            for (auto&& nrp: n.relations()) {
                const auto& nr = nrp.second.get();
                out << YAML::Key << to_string(nrp.first) << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "id" << YAML::Value << nodes.at(&nr);
#define EMIT_NAV(name) \
                if (nr. name ()) { \
                    out << YAML::Key << #name << YAML::Value << nodes.at(nr. name ()); \
                }
                EMIT_NAV(next)
                EMIT_NAV(prev)
                EMIT_NAV(parent)
                EMIT_NAV(first_child)
                EMIT_NAV(last_child)
                out << YAML::EndMap;
            }
            out << YAML::EndMap << YAML::EndMap;
        }
    }
    out << YAML::EndSeq << YAML::Key << "relations" << YAML::Value << YAML::BeginMap;
    for (auto&& rel: g) {
        auto& nr = *rel.second;
        out << YAML::Key << to_string(nr.name()) << YAML::Value << YAML::BeginMap;
        EMIT_NAV(first)
        EMIT_NAV(last)
        out << YAML::EndMap;
    }
    out << YAML::EndMap << YAML::EndMap;
    return out;
}

std::ostream& operator << (std::ostream& os, const graph& g) {
    YAML::Emitter out{os};
    out << g;
    return os;
}

std::istream& operator >> (std::istream& is, graph& g) {
    auto res = graph::from_stream(is, g.to_builder());
    if (!is.fail()) {
        static_cast<graph::base&>(g) = std::move(res);
    }
    return is;
}
} // namespace hrglib
