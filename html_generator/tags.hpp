#include <silicium/html/tree.hpp>
#include <boost/intrusive/options.hpp>

namespace
{

	//----------------TITLE tag----------------
	template <class Element>
	auto html(Element &&content)
	{
		return Si::html::tag("html", std::forward<Element>(content));
	}

	template <class Element>
	auto head(Element &&content)
	{
		return Si::html::tag("head", std::forward<Element>(content));
	}

	template <class Element>
	auto body(Element &&content)
	{
		return Si::html::tag("body", std::forward<Element>(content));
	}

	//----------------TITLE tag----------------
	template <class Element>
	auto title(Element &&content)
	{
		return Si::html::tag("title", std::forward<Element>(content));
	}
	template <class StringLike>
	auto title(StringLike &content)
	{
		return title(Si::html::text(content));
	}

	//----------------H1 tag----------------
	template <class Element>
	auto h1(Element &&content)
	{
		return Si::html::tag("h1", std::forward<Element>(content));
	}
	template <class StringLike>
	auto h1(StringLike &content)
	{
		return h1(Si::html::text(content));
	}

	//----------------H2 tag----------------
	template <class Element>
	auto h2(Element &&content)
	{
		return Si::html::tag("h2", std::forward<Element>(content));
	}

	template <class StringLike>
	auto h2(StringLike &content)
	{
		return h2(Si::html::text(content));
	}

	//----------------H3 tag----------------
	template <class Element>
	auto h3(Element &&content)
	{
		return Si::html::tag("h3", std::forward<Element>(content));
	}

	template <class StringLike>
	auto h3(StringLike &content)
	{
		return h3(Si::html::text(content));
	}

	//----------------H4 tag----------------
	template <class Element>
	auto h4(Element &&content)
	{
		return Si::html::tag("h4", std::forward<Element>(content));
	}

	template <class StringLike>
	auto h4(StringLike &content)
	{
		return h4(Si::html::text(content));
	}

	//----------------MENU tag----------------
	template <class Element>
	auto menu(Element &&content)
	{
		return Si::html::tag("menu", std::forward<Element>(content));
	}

	//----------------FOOTER tag----------------
	template <class Element>
	auto footer(Element &&content)
	{
		return Si::html::tag("footer", std::forward<Element>(content));
	}

	//----------------P tag----------------
	template <class Element>
	auto p(Element &&content)
	{
		return Si::html::tag("p", std::forward<Element>(content));
	}

	template <class StringLike>
	auto p(StringLike const &content)
	{
		return p(Si::html::text(content));
	}

	template <class Element, class Attributes>
	auto p(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("p", std::forward<Attributes>(attributes),
		                     std::forward<Element>(content));
	}

	//----------------DIV tag----------------
	template <class Element>
	auto div(Element &&content)
	{
		return Si::html::tag("div", std::forward<Element>(content));
	}

	template <class StringLike>
	auto div(StringLike const &content)
	{
		return div(p(Si::html::text(content)));
	}

	template <class Element, class Attributes>
	auto div(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("div", std::forward<Attributes>(attributes),
		                     std::forward<Element>(content));
	}

	//----------------SPAN tag----------------
	template <class Element>
	auto span(Element &&content)
	{
		return Si::html::tag("span", std::forward<Element>(content));
	}

	template <class Element, class Attributes>
	auto span(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("span", std::forward<Attributes>(attributes),
		                     std::forward<Element>(content));
	}

	//----------------table tags----------------
	template <class Element>
	auto table(Element &&content)
	{
		return Si::html::tag("table", std::forward<Element>(content));
	}

	template <class StringLike, class Element>
	auto table(StringLike summary, Element &&content)
	{
		return Si::html::tag("table", Si::html::attribute("summary", summary),
		                     std::forward<Element>(content));
	}

	template <class Element>
	auto thead(Element &&content)
	{
		return Si::html::tag("thead", std::forward<Element>(content));
	}
	template <class Element>
	auto tbody(Element &&content)
	{
		return Si::html::tag("tbody", std::forward<Element>(content));
	}
	template <class Element>
	auto tfoot(Element &&content)
	{
		return Si::html::tag("tfoot", std::forward<Element>(content));
	}

	//----------------TD tag----------------
	template <class Element>
	auto td(Element &&content)
	{
		return Si::html::tag("td", std::forward<Element>(content));
	}
	template <class StringLike>
	auto td(StringLike const &content)
	{
		return td(Si::html::text(content));
	}

	//----------------TH tag----------------
	template <class Element>
	auto th(Element &&content)
	{
		return Si::html::tag("th", std::forward<Element>(content));
	}
	template <class StringLike>
	auto th(StringLike const &content)
	{
		return th(Si::html::text(content));
	}

	//----------------UL tag----------------
	template <class Element>
	auto ul(Element &&content)
	{
		return Si::html::tag("ul", std::forward<Element>(content));
	}

	template <class Element, class Attributes>
	auto ul(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("ul", std::forward<Attributes>(attributes),
		                     std::forward<Element>(content));
	}
	//----------------LI tag----------------
	template <class Element>
	auto li(Element &&content)
	{
		return Si::html::tag("li", std::forward<Element>(content));
	}

	template <class StringLike>
	auto li(StringLike const &content)
	{
		return li(Si::html::text(content));
	}

	//----------------PRE tag----------------
	template <class Element>
	auto pre(Element &&content)
	{
		return Si::html::tag("pre", std::forward<Element>(content));
	}

	template <class Element, class Attributes>
	auto pre(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("pre", std::forward<Attributes>(attributes),
		                     std::forward<Element>(content));
	}

	//----------------BR tag----------------
	auto br()
	{
		using namespace Si::html;
		return tag("br", empty);
	}

	//----------------IMG tag----------------
	template <class StringLike>
	auto img(StringLike const &content)
	{
		using namespace Si::html;
		return tag("img", attribute("src", content), empty);
	}

	//----------------classes attrib----------------
	template <class StringLike>
	auto cl(StringLike const &content)
	{
		return Si::html::attribute("class", content);
	}

	//----------------id attrib----------------
	template <class StringLike>
	auto id(StringLike const &content)
	{
		return Si::html::attribute("id", content);
	}

	//----------------href attrib----------------
	template <class StringLike>
	auto href(StringLike const &content)
	{
		return Si::html::attribute("href", content);
	}

	// PSEUDO TAG: link (emulates a tag)
	template <std::size_t N>
	auto link(std::string const &protocol,
	          char const(&address_without_protocol)[N], std::string caption)
	{
		using namespace Si::html;
		return dynamic(
		    [protocol, address_without_protocol, caption](code_sink &sink)
		    {
			    if (protocol == "http://" || protocol == "https://")
			    {
				    tag("a", href(protocol + address_without_protocol) +
				                 attribute("target", "_blank"),
				        text(caption))
				        .generate(sink);
			    }
			    else
			    {
				    tag("a", href(protocol + address_without_protocol),
				        text(caption))
				        .generate(sink);
			    }
			});
	}

	// PSEUDO TAG: link (emulates a tag)
	template <std::size_t N>
	auto link(std::string const &protocol,
	          char const(&address_without_protocol)[N])
	{
		return link(protocol, address_without_protocol,
		            address_without_protocol);
	}

	// PSEUDO ATTRIBUTE: anchor_attributes (emulates a jump mark on a page)
	template <std::size_t N>
	auto anchor_attributes(char const(&name)[N])
	{
		using namespace Si::html;
		return attribute("name", name) + href(std::string("#") + name);
	}
}
