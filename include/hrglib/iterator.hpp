/**
 * @file hrglib/iterator.hpp
 * @brief Iterator utilities, `hrglib::range` and proxy header to <iterator>.
 */
#pragma once
#include <iterator>  // IWYU pragma: export

namespace hrglib {
//! @brief Iterator adapters & related utilities.
namespace iterator {
template<typename Iterator> struct sentinel {};

template<class DerivedIterator>
struct with_sentinel {
    using sentinel = iterator::sentinel<DerivedIterator>;

    friend constexpr bool operator!=(const DerivedIterator& lhs, sentinel) noexcept {
        return lhs.has_next();
    }
    friend constexpr bool operator==(const DerivedIterator& lhs, sentinel) noexcept {
        return !lhs.has_next();
    }
    friend constexpr bool operator!=(sentinel, const DerivedIterator& rhs) noexcept {
        return rhs.has_next();
    }
    friend constexpr bool operator==(sentinel, const DerivedIterator& rhs) noexcept {
        return !rhs.has_next();
    }
};

template<class DerivedIterator>
struct with_reverse_sentinel {
    using reverse_sentinel = sentinel<std::reverse_iterator<DerivedIterator>>;

    friend constexpr bool operator==(const std::reverse_iterator<DerivedIterator>& lhs, reverse_sentinel) noexcept {
        return !lhs.base().has_prev();
    }
    friend constexpr bool operator!=(const std::reverse_iterator<DerivedIterator>& lhs, reverse_sentinel) noexcept {
        return lhs.base().has_prev();
    }
    friend constexpr bool operator==(reverse_sentinel, const std::reverse_iterator<DerivedIterator>& rhs) noexcept {
        return !rhs.base().has_prev();
    }
    friend constexpr bool operator!=(reverse_sentinel, const std::reverse_iterator<DerivedIterator>& rhs) noexcept {
        return rhs.base().has_prev();
    }
};

template<typename BeginType, typename EndType = BeginType>
class range {
    BeginType begin_;
    EndType end_;
public:
    constexpr range(BeginType begin, EndType end):
        begin_{std::move(begin)}, end_{std::move(end)} {}

    constexpr BeginType begin() const { return begin_; }
    constexpr EndType end() const { return end_; }
};

#if (__cpp_deduction_guides >= 201703L)
template<typename BeginType, typename EndType>
range(BeginType begin, EndType end) -> range<BeginType, EndType>;
#endif

template<typename Iterator, typename SentinelType = typename Iterator::sentinel>
class range_with_sentinel: public range<Iterator, SentinelType> {
    using base = range<Iterator, SentinelType>;
public:
    constexpr range_with_sentinel(Iterator begin): base{std::move(begin), {}} {}
};

template<typename Iterator, typename ReverseSentinelType = typename Iterator::reverse_sentinel>
class reverse_range_with_sentinel: public range_with_sentinel<std::reverse_iterator<Iterator>, ReverseSentinelType> {
    using base = range_with_sentinel<std::reverse_iterator<Iterator>, ReverseSentinelType>;
public:
    constexpr reverse_range_with_sentinel(Iterator rbegin):
        base{std::make_reverse_iterator(std::move(rbegin))} {}
};
}}  // namespace hrglib::iterator
