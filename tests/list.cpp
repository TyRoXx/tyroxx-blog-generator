#include "html_generator/tags.hpp"
#include "html_generator/tools/list.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(empty_unordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	unordered_list().generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<ul></ul>", html_generated);
}

BOOST_AUTO_TEST_CASE(empty_ordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	ordered_list().generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<ol></ol>", html_generated);
}

BOOST_AUTO_TEST_CASE(single_unordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	unordered_list(Si::html::text("Hello World")).generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<ul><li>Hello World</li></ul>", html_generated);
}

BOOST_AUTO_TEST_CASE(single_ordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	ordered_list(Si::html::text("Hello World")).generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<ol><li>Hello World</li></ol>", html_generated);
}

BOOST_AUTO_TEST_CASE(long_unordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	unordered_list(Si::html::text("Cheese"), Si::html::text("Bacon"),
	               Si::html::text("Bread"), Si::html::text("Milk"))
	    .generate(erased_html_sink);
	BOOST_CHECK_EQUAL(
	    "<ul><li>Cheese</li><li>Bacon</li><li>Bread</li><li>Milk</li></ul>",
	    html_generated);
}

BOOST_AUTO_TEST_CASE(long_ordered)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	ordered_list(Si::html::text("Make the dough"),
	             Si::html::text("Put toping on the pizza"),
	             Si::html::text("Put it into the oven"),
	             Si::html::text("Eat it"))
	    .generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<ol><li>Make the dough</li><li>Put toping on the "
	                  "pizza</li><li>Put it into the oven</li><li>Eat "
	                  "it</li></ol>",
	                  html_generated);
}