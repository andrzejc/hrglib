#include "hrglib/graph.hpp"
#include "hrglib/node.hpp"
#include "hrglib/error.hpp"
#include "hrglib/token.hpp"
#include "hrglib/word.hpp"
#include "hrglib/relation.hpp"
#include "hrglib/relation_name.hpp"
#include "hrglib/string.hpp"

#include "utils.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
namespace hrglib::test {

TEST(graph, from_file_success) {
    auto g = graph::from_file(data_file("test_graph.yaml"));
    EXPECT_TRUE(g.has(R::Token));
    EXPECT_EQ(g.at<R::Token>().size(), 2);
    EXPECT_NE(g.at<R::Token>().first(), nullptr);
    EXPECT_NE(g.at<R::Token>().last(), nullptr);
    EXPECT_TRUE(g.has(R::Word));
    EXPECT_EQ(g.at<R::Word>().size(), 2);
    EXPECT_NE(g.at<R::Word>().first(), nullptr);
    EXPECT_NE(g.at<R::Word>().last(), nullptr);
    EXPECT_EQ(g.at<R::Word>().first().parent(), g.at<R::Token>().first());
}

TEST(graph, get) {
    graph g;
    EXPECT_FALSE(g.get(R::Token));
    EXPECT_FALSE(g.get<R::Token>());
    EXPECT_FALSE(as_const(g).get(R::Word));
    EXPECT_FALSE(as_const(g).get<R::Word>());

    g.at<relation_t<R::Token>>();

    EXPECT_TRUE(g.get(R::Token));
    EXPECT_TRUE(g.get<R::Token>());
    EXPECT_TRUE(as_const(g).get(R::Token));
    EXPECT_TRUE(as_const(g).get<R::Token>());

    EXPECT_FALSE(g.get(R::Word));
}

TEST(graph, at) {
    graph g;
    EXPECT_FALSE(g.get(R::Token));

    EXPECT_NE(dynamic_cast<relation_t<R::Token>*>(&g.at(R::Token)), nullptr);
    EXPECT_TRUE(g.get(R::Token));
}

TEST(graph, from_file_throws) {
    EXPECT_THROW(graph::from_file(data_file("foo.json")), std::runtime_error);
}

TEST(graph, from_string_success) {
    EXPECT_NO_THROW(graph::from_string("{ }"));
}

TEST(graph, from_string_throws) {
    EXPECT_THROW(graph::from_string(""), std::runtime_error);
    EXPECT_THROW(graph::from_string("[ \"foo\" ]"), std::runtime_error);
}

TEST(graph, format) {
    std::cerr << graph::from_file(data_file("test_graph.yaml")) << std::endl;
}
}  // namespace hrglib::test
