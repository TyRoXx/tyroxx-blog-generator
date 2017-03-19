#include "html_generator/cpp_syntax_highlighting.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
	void check_code_rendering(std::string cpp, std::string const &html_expected)
	{
		std::string html_generated;
		auto erased_html_sink = Si::Sink<char, Si::success>::erase(
		    Si::make_container_sink(html_generated));
		auto tree = render_code_raw(std::move(cpp));
		tree.generate(erased_html_sink);
		BOOST_CHECK_EQUAL(html_expected, html_generated);
	}
}

BOOST_AUTO_TEST_CASE(render_code_raw_empty)
{
	check_code_rendering("", "");
}

BOOST_AUTO_TEST_CASE(render_code_raw_string)
{
	check_code_rendering(
	    "\"my string\"",
	    R"(<span class="stringLiteral">&quot;my string&quot;</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_string_escape_quotes)
{
	check_code_rendering(
	    "\"my string\\\"\"",
	    R"(<span class="stringLiteral">&quot;my string\&quot;&quot;</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_string_escape_backslash)
{
	check_code_rendering(
	    "\"my string\\\\\"",
	    R"(<span class="stringLiteral">&quot;my string\\&quot;</span>)");
}
