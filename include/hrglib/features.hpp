/**
 * @file hrglib/features.hpp
 * @brief Definition of `hrglib::features` collection.
 */
#pragma once
#include "hrglib/feature_name.hpp"
#include "hrglib/feature_traits.hpp"
#include "hrglib/optional.hpp"
#include "hrglib/string.hpp"
#include "hrglib/any.hpp"
#include "hrglib/map_find.hpp"

#ifdef HRGLIB_YAMLCPP_PUBLIC
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/emitter.h>
#endif

#include <unordered_map>
#include <functional>
#include <iosfwd>

namespace hrglib {
/**
 * @brief Key-value collection storing arbitrarily typed values inside `any` instances
 *     and providing type-safe access to them through use of `feature_traits`
 *     specializations.
 */
class features: private std::unordered_map<feature_name, any> {
    using base = std::unordered_map<feature_name, any>;

    //! @brief Insert-or-access operation which ensures that `any` instance inserted at
    //!     @p feat is always initialized to contain `feature_t<feat>`, thus avoiding
    //!     `std::bad_any_cast` when used to instantiate a new value.
    template<feature_name feat>
    any& at_() {
        if (auto res = map_find<base>(*this, feat)) {
            return *res;
        } else {
            return base::emplace(feat, feature_t<feat>{}).first->second;
        }
    }

    //! @brief Common implementation of both non-static (const and non-const) @c get() members.
    //! @tparam Features (possibly const) `features` whose constness will be applied to the
    //!     returned `optional`.
    //! @return reference to value stored at @p feat or `nullopt` if no such feature.
    template<feature_name feat, class Features>
    static optional<std::add_lvalue_reference_t<copy_const_t<Features, feature_t<feat>>>> get_(Features& feats) {
        if (auto res = map_find<copy_const_t<Features, base>>(feats, feat)) {
            return {any_cast<std::add_lvalue_reference_t<copy_const_t<Features, feature_t<feat>>>>(*res)};
        } else {
            return {};
        }
    }

public:
    //! Feature presence check.
    //! @param feat to check.
    //! @return `true` if @p feat is present.
    bool has(feature_name feat) const noexcept { return !!get(feat); }
    /**
     * @brief Templated access or insert element at key @p feat specified at compile-time.
     *
     * The type of returned reference is inferred from `feature_traits` for given @p feat.
     * If there is no such element prior the call, `any` instance default-initialized with object of
     * type `feature_t<feat>` will be inserted and its reference will be used. This allows to use
     * the result as l-value in assignment operation.
     *
     * @tparam feat Compile-time constant specifing feature label to select.
     * @return `feature_t<feat>&`
     */
    template<feature_name feat>
    feature_t<feat>& at() { return any_cast<feature_t<feat>&>(at_<feat>()); }
    /**
     * @brief Optional immutable acces to raw `any` container at runtime-selectable @p feat.
     *
     * @param feat to access.
     * @return `optional<const any&>`
     */
    optional<const any&> get(feature_name feat) const { return map_find<const base>(*this, feat); }
    /**
     * @brief Optional immutable acces to feature value at compile-time selectable @p feat.
     *
     * @tparam feat to access.
     * @return `optional<const feature_t<feat>&>`
     */
    template<feature_name feat>
    optional<const feature_t<feat>&> get() const { return get_<feat>(*this); }
    /**
     * @brief Optional acces to feature value at compile-time selectable @p feat.
     *
     * @tparam feat to access.
     * @return `optional<feature_t<feat>&>`
     */
    template<feature_name feat>
    optional<feature_t<feat>&> get() { return get_<feat>(*this); }
    /**
     * @brief Set value at compile-time selectable @p feat in a type-safe way.
     *
     * @tparam feat to access.
     * @tparam ValueType Arbitrary type assignable to `feature_t<feat>` lvalue.
     * @param value forwarding reference to whatever value was given as input.
     * @return `*this` for builder-like chaining.
     */
    template<feature_name feat, typename ValueType>
    features& set(ValueType&& value) {
        at<feat>() = std::forward<ValueType>(value);
        return *this;
    }
    /**
     * @brief Remove feature at compile-time selectable @p feat and optinally return its value.
     *
     * @tparam feat to remove.
     * @return `optional<feature_t<feat>>` with value of removed feature_name moved-in.
     */
    template<feature_name feat>
    optional<feature_t<feat>> remove() {
        if (auto it = base::find(feat); it != base::end()) {
            optional<feature_t<feat>> res = any_cast<feature_t<feat>&&>(std::move(it->second));
            base::erase(it);
            return res;
        } else {
            return {};
        }
    }

    using const_iterator = base::const_iterator;
    // Disable write access via iterators - this would allow to inject
    // wrongly-typed std::any's into the map, breaking class invariants.
    using iterator = const_iterator;

    const_iterator begin() const { return base::begin(); }
    iterator begin() { return std::as_const(*this).begin(); }
    const_iterator cbegin() const { return base::cbegin(); }

    const_iterator end() const { return base::end(); }
    iterator end() { return std::as_const(*this).end(); }
    const_iterator cend() const { return base::cend(); }

    const_iterator find(feature_name key) const { return base::find(key); }

    // std::unordered_map APIs which don't break features invariants.
    using base::erase;
    using base::size;
    using base::empty;
    using base::clear;

    //! @brief Type of function mapping textual feature name labels to `feature` enum constants.
    using name_mapper_type = std::function<feature_name(string_view)>;
    //! @brief Default implementation of `name_mapper_type`, alias to `from_string<feature_name>`.
    static constexpr auto DEFAULT_NAME_MAPPER = hrglib::from_string<feature_name>;

    static features from_file(
            string_view path,
            const name_mapper_type& feature_name_mapper = nullptr);

    static features from_stream(
            std::istream& is,
            const name_mapper_type& feature_name_mapper = nullptr);

    static features from_string(
            string_view yaml,
            const name_mapper_type& feature_name_mapper = nullptr);

#ifdef HRGLIB_YAMLCPP_PUBLIC
    static features from_yaml(
            const YAML::Node& document,
            const name_mapper_type& feature_name_mapper = nullptr);

    friend YAML::Emitter& operator << (YAML::Emitter& lhs, const features& rhs);
#endif

    friend std::ostream& operator << (std::ostream& lhs, const features& rhs);
    friend std::istream& operator >> (std::istream& lhs, features& rhs);
};
}  // namespace hrglib
