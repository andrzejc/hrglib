#include "hrglib/node.hpp"
#include "hrglib/token.hpp"
#include "hrglib/word.hpp"
#include "hrglib/syllable.hpp"
#include "hrglib/error.hpp"
#include "hrglib/graph.hpp"
#include "hrglib/node_navigator.hpp"
#include "hrglib/relation.hpp"
#include "hrglib/relation_name.hpp"

#include <gtest/gtest.h>

#include <type_traits>                // for is_same_v

namespace hrglib::test {

TEST(node, null_dereference) {
    graph g;
    auto& n = g.at(R::Token).create();
    static_assert(std::is_same_v<decltype(n), node&>, "");
    EXPECT_THROW(*n.next(), error::null_navigator_dereference);
    EXPECT_THROW(*n.prev(), error::null_navigator_dereference);
    EXPECT_THROW(*n.parent(), error::null_navigator_dereference);
    EXPECT_THROW(*n.first_child(), error::null_navigator_dereference);
    EXPECT_THROW(*n.last_child(), error::null_navigator_dereference);
    EXPECT_THROW(*n.as(R::Word), error::null_navigator_dereference);
    EXPECT_THROW(*n.as<R::Word>(), error::null_navigator_dereference);
    EXPECT_THROW(*n.as<word>(), error::null_navigator_dereference);
    // operator-> performs dereference to invoke node method
    EXPECT_THROW(n.next()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.prev()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.parent()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.first_child()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.last_child()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.as(R::Word)->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.as<R::Word>()->next(), error::null_navigator_dereference);
    EXPECT_THROW(n.as<word>()->next(), error::null_navigator_dereference);
}

TEST(node_navigator, traversal_does_not_throw) {
    graph g;
    auto& n = g.at<R::Token>().create();
    EXPECT_NO_THROW(n.next().prev().as<R::Token>()
            .first_child().prev().first_child().as<R::Word>()
            .parent().next().as<R::Token>().next().next()
            .last_child().as<R::Word>().next().next().next().first_child()
            .next().as<R::Syllable>().parent().as<R::Syllable>()
            .apply([](syllable& s){ return s.parent(); }).next()) <<
            "navigator chaining never throws if not dereferenced";
}

graph make_simple_graph() {
    graph g;
    auto& t1 = g.at<R::Token>().create();
    auto& w1 = g.at<R::Word>().create();
    t1.set_first_child(&w1);
    t1.set_last_child(&w1);

    auto& w2 = g.at<R::Word>().create();
    w2.set_parent(&t1);
    w1.set_next(&w2);

    auto& w3 = g.at<R::Word>().create();
    t1.set_last_child(&w3);
    w3.set_prev(&w2);
    return g;
}

TEST(node, as) {
    graph g;
    auto& t = g.at<R::Token>().create();
    // attach node handle in R::Word to existing R::Token
    auto& w = g.at<R::Word>().create(&t);

    EXPECT_EQ(t.as<R::Syllable>(), nullptr);
    EXPECT_EQ(w.as<syllable>(), nullptr);

    // and another one in R::syllabe
    EXPECT_EQ(t.as<R::Word>(), &w);
    EXPECT_EQ(w.as<R::Token>(), &t);

    EXPECT_EQ(t.as<word>(), &w);
    EXPECT_EQ(w.as<token>(), &t);

    auto& s = g.at<R::Syllable>().create(&t);
    EXPECT_EQ(t.as<R::Syllable>(), &s);
    EXPECT_EQ(s.as<R::Token>(), &t);

    g.at<R::Word>().erase(w);
    EXPECT_EQ(t.as<R::Word>(), nullptr);
    EXPECT_EQ(s.as<word>(), nullptr);
}

// TEST(node, insert_next_prev) {
//     auto g = make_simple_graph();
//     auto w4 = g.at<R::Word>().insert_next();
//     EXPECT_EQ(g[2]->next(), w4.get());
//     EXPECT_EQ(g[3]->prev(), w4.get());
//     EXPECT_EQ(w4->prev(), g[2].get());
//     EXPECT_EQ(w4->next(), g[3].get());

//     auto w5 = g[2]->insert_prev();
//     EXPECT_EQ(g[2]->prev(), w5.get());
//     EXPECT_EQ(w5->next(), g[2].get());
//     // w5 is inserted at the end, so has no next.
//     // EXPECT_EQ(w5->prev(), nullptr);
// }

// TEST(node, iter) {
//     auto g = make_simple_graph();
//     for (auto&& n: g[0]->iter()) {
//         EXPECT_EQ(n, *g[0]);
//     }
//     auto count = 0;
//     for (auto&& n: g[1]->iter()) {
//         ++count;
//     }
//     EXPECT_EQ(count, 3);

//     count = 0;
//     for (auto&& n: g[1]->iter_back()) {
//         ++count;
//     }
//     EXPECT_EQ(count, 0);

//     count = 0;
//     for (auto&& n: g[3]->iter()) {
//         ++count;
//     }
//     EXPECT_EQ(count, 1);

//     count = 0;
//     for (auto&& n: g[3]->iter_back()) {
//         ++count;
//     }
//     EXPECT_EQ(count, 2);

//     count = 0;
//     for (auto&& w: g[1]->as<word>()->iter()) {
//         static_assert(std::is_same<word&, decltype(w)>::value, "");
//         ++count;
//     }
//     EXPECT_EQ(count, 3);
// }

}  // namespace hrglib
