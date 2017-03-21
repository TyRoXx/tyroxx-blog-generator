#include "html_generator/tags.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(tags_link)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	auto tree = tags::link("http://", "google.com");
	tree.generate(erased_html_sink);
	BOOST_CHECK_EQUAL(
	    R"(<a href="http://google.com" target="_blank">google.com</a>)",
	    html_generated);
}
