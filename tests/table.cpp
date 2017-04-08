#include "html_generator/tags.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
	namespace detail
	{
		template <class Element>
		auto join_table_cells(Element &&single)
		{
			return tags::td(std::forward<Element>(single));
		}

		template <class Element1, class Element2, class... ElementTail>
		auto join_table_cells(Element1 &&first, Element2 &&second,
		                      ElementTail &&... tail)
		{
			return tags::td(std::forward<Element1>(first)) +
			       join_table_cells(std::forward<Element2>(second),
			                        std::forward<ElementTail>(tail)...);
		}
	}

	template <class... Elements>
	auto row(Elements &&... cells)
	{
		return tags::tr(
		    detail::join_table_cells(std::forward<Elements>(cells)...));
	}
}

BOOST_AUTO_TEST_CASE(table_manually)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	tags::table(tags::tr(tags::td(Si::html::text("123")) +
	                     tags::td(Si::html::text("abc"))) +
	            tags::tr(tags::td(Si::html::text("456")) +
	                     tags::td(Si::html::text("def"))))
	    .generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<table><tr><td>123</td><td>abc</td></tr><tr><td>456</"
	                  "td><td>def</td></tr></table>",
	                  html_generated);
}

BOOST_AUTO_TEST_CASE(table_syntactic_sugar)
{
	std::string html_generated;
	auto erased_html_sink = Si::Sink<char, Si::success>::erase(
	    Si::make_container_sink(html_generated));
	tags::table(row(Si::html::text("123"), Si::html::text("abc")) +
	            row(Si::html::text("456"), Si::html::text("def")))
	    .generate(erased_html_sink);
	BOOST_CHECK_EQUAL("<table><tr><td>123</td><td>abc</td></tr><tr><td>456</"
	                  "td><td>def</td></tr></table>",
	                  html_generated);
}
