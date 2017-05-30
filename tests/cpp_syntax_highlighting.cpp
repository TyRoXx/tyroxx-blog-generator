#include "html_generator/tools/cpp_syntax_highlighting.hpp"
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
	check_code_rendering("const",
	                     R"(<span class="keyword">const</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_scoped_identifier)
{
	check_code_rendering("scope::identifier",
	                     R"(<span class="names">scope::identifier</span>)");
}

BOOST_AUTO_TEST_CASE(render_code_raw_global_scoped_identifier)
{
	check_code_rendering("::scope::identifier",
	                     R"(<span class="names">::scope::identifier</span>)");
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
        check_code_rendering(<span class="stringLiteral">&quot;\&quot;abc\&apos;&quot;</span>, <span class="stringLiteral">&quot;&quot;</span>), <span class="names">std::invalid_argument</span>,
        [](<span class="names">std::invalid_argument</span> <span class="keyword">const</span> &amp;ex)
        {
            <span class="keyword">return</span> (<span class="names">std::string</span>(<span class="stringLiteral">&quot;Number of quotes must be even&quot;</span>) == ex.what());
        });
})");
}

BOOST_AUTO_TEST_CASE(render_code_raw_include)
{
	check_code_rendering(
	    "#include <vector>\n",
	    "<span class=\"preprocessor\">#include &lt;vector&gt;</span>\n");
}

BOOST_AUTO_TEST_CASE(render_an_empty_comment)
{
	check_code_rendering("code;// \n",
	                     "code;<span class=\"comment\">// </span>\n");
}

BOOST_AUTO_TEST_CASE(render_a_comment)
{
	check_code_rendering("//Hello testing it\n",
	                     "<span class=\"comment\">//Hello testing it</span>\n");
}

BOOST_AUTO_TEST_CASE(render_an_empty_block_comment)
{
	check_code_rendering("/**/\n", "<span class=\"comment\">/**/</span>\n");
}

BOOST_AUTO_TEST_CASE(render_a_block_comment)
{
	check_code_rendering(
	    "/*This is some test code and the bool should not be rendered as a "
	    "keyword\nNeither should a line break confuse it*/\n",
	    "<span class=\"comment\">/*This is some test code and the bool should "
	    "not be rendered as a keyword\nNeither should a line break confuse "
	    "it*/</span>\n");
}

BOOST_AUTO_TEST_CASE(render_a_doc_comment)
{
	check_code_rendering("/**Special documentation comment incoming*/\n",
	                     "<span class=\"comment\">/**Special documentation "
	                     "comment incoming*/</span>\n");
}
