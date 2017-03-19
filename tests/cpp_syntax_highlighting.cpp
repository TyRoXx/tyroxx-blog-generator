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

BOOST_AUTO_TEST_CASE(render_code_raw_char_literal)
{
	check_code_rendering("\'a\'",
	                     R"(<span class="stringLiteral">&apos;a&apos;</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_identifier)
{
	check_code_rendering("identifier",
	                     R"(identifier)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_keyword)
{
	check_code_rendering(
	    "const",
	    R"(<span class="keyword">const</span><span class="names">const</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_scoped_identifier)
{
	check_code_rendering(
	    "scope::identifier",
	    R"(<span class="names">scope</span><span class="names">::</span><span class="names">identifier</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_global_scoped_identifier)
{
	check_code_rendering(
	    "::scope::identifier",
	    R"(<span class="names">::</span><span class="names">scope</span><span class="names">::</span><span class="names">identifier</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_space)
{
	check_code_rendering(" \t\r\n\n", " \t\r\n\n");
}

BOOST_AUTO_TEST_CASE(render_code_raw_empty_line)
{
	check_code_rendering("\na", "\na");
}

BOOST_AUTO_TEST_CASE(render_code_raw_single_colon)
{
	check_code_rendering(":", ":");
}

BOOST_AUTO_TEST_CASE(render_code_raw_colon_prefix)
{
	check_code_rendering(":a", ":a");
}

BOOST_AUTO_TEST_CASE(render_code_raw_braces)
{
	check_code_rendering("{}", "{}");
}

BOOST_AUTO_TEST_CASE(render_code_raw_mismatched_quotes)
{
	BOOST_CHECK_EXCEPTION(
	    check_code_rendering("\"abc\'", ""), std::invalid_argument,
	    [](std::invalid_argument const &ex)
	    {
		    return (std::string("Number of quotes must be even") == ex.what());
		});
}

BOOST_AUTO_TEST_CASE(render_code_raw_realistic_example)
{
	check_code_rendering(
	    R"(BOOST_AUTO_TEST_CASE(render_code_raw_mismatched_quotes)
{
	BOOST_CHECK_EXCEPTION(
	    check_code_rendering("\"abc\'", ""), std::invalid_argument,
	    [](std::invalid_argument const &ex)
	    {
		    return (std::string("Number of quotes must be even") == ex.what());
		});
})",
	    R"(BOOST_AUTO_TEST_CASE(render_code_raw_mismatched_quotes)
{
	BOOST_CHECK_EXCEPTION(
	    check_code_rendering(<span class="stringLiteral">&quot;\&quot;abc\&apos;&quot;</span>, <span class="stringLiteral">&quot;&quot;</span>), <span class="names">std</span><span class="names">::</span><span class="names">invalid_argument</span>,
	    [](<span class="names">std</span><span class="names">::</span><span class="names">invalid_argument</span> <span class="keyword">const</span><span class="names">const</span> &amp;ex)
	    {
		    <span class="keyword">return</span><span class="names">return</span> (<span class="names">std</span><span class="names">::</span><span class="names">string</span>(<span class="stringLiteral">&quot;Number of quotes must be even&quot;</span>) == ex.what());
		});
})");
}

BOOST_AUTO_TEST_CASE(render_code_raw_include)
{
	check_code_rendering("#include <vector>\n", "#include &lt;vector&gt;\n");
}
