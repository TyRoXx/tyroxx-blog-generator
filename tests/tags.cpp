#include "html_generator/tags.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(tags_link)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	tags::link("https://", "google.com").generate(erased_html_sink);
	BOOST_CHECK_EQUAL(
	    "<a href=\"https://google.com\" target=\"_blank\">google.com</a>",
	    html_generated);
}

BOOST_AUTO_TEST_CASE(paragraph)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	tags::p("Testing p tag").generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<p>Testing p tag</p>", html_generated);
}

BOOST_AUTO_TEST_CASE(break_row)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	tags::br(tags::cl("clear")).generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<br class=\"clear\"/>", html_generated);
}
