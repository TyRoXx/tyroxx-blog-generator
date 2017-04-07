#include <boost/test/unit_test.hpp>
#include <silicium/success.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include "../html_generator/blog_lang.hpp"

namespace
{
	void check_code_rendering(std::string cpp, std::string const &html_expected)
	{
		std::string html_generated;
		auto erased_html_sink = Si::Sink<char, Si::success>::erase(
		    Si::make_container_sink(html_generated));
		auto tree = compile(std::move(cpp));
		tree.generate(erased_html_sink);
		BOOST_CHECK_EQUAL(html_expected, html_generated);
	}
}

BOOST_AUTO_TEST_CASE(render_heading)
{
	check_code_rendering("#This is a heading\n", "<h1>This is a heading</h1>");
}

BOOST_AUTO_TEST_CASE(render_text)
{
	check_code_rendering("This is normal text\n", "<p>This is normal text</p>");
}

BOOST_AUTO_TEST_CASE(render_inline_code)
{
	check_code_rendering("Code: `int i = 0;`",
	                     "<p>Code: </p><span "
	                     "class=\"inlineCodeSnippet\"><code><span "
	                     "class=\"keyword\">int</span> i = 0;</code></span>");
}