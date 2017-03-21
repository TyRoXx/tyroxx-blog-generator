#pragma once
#include <silicium/html/tree.hpp>

namespace tags
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
	inline auto title(std::string const &content)
	{
		using namespace Si::html;
		return tag("title", text(content));
	}

	//----------------H1 tag----------------
	template <class Element>
	auto h1(Element &&content)
	{
		using namespace Si::html;
		return tag("h1", std::forward<Element>(content));
	}

	//----------------H2 tag----------------
	template <class Element>
	auto h2(Element &&content)
	{
		using namespace Si::html;
		return tag("h2", std::forward<Element>(content));
	}

	//----------------H3 tag----------------
	template <class Element>
	auto h3(Element &&content)
	{
		using namespace Si::html;
		return tag("h3", std::forward<Element>(content));
	}

	//----------------H4 tag----------------
	template <class Element>
	auto h4(Element &&content)
	{
		using namespace Si::html;
		return tag("h4", std::forward<Element>(content));
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
	inline auto p(std::string const &content)
	{
		using namespace Si::html;
		return tag("p", text(content));
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

	template <class Element>
	auto table(std::string const &summary, Element &&content)
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

	template <class Attributes>
	auto br(Attributes &&attributes)
	{
		using namespace Si::html;
		return tag("br", std::forward<Attributes>(attributes), empty);
	}

	//----------------classes attrib----------------
	inline auto cl(std::string const &content)
	{
		return Si::html::attribute("class", content);
	}

	//----------------href attrib----------------
	inline auto href(std::string const &content)
	{
		return Si::html::attribute("href", content);
	}

	// PSEUDO TAG: link (emulates the a-tag)
	template <std::size_t N>
	auto link(std::string const &protocol,
	          char const(&address_without_protocol)[N],
	          std::string const &caption)
	{
		using namespace Si::html;
		return dynamic(
		    [protocol, &address_without_protocol, caption](code_sink &sink)
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
