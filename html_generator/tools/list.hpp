#pragma once

#include <html_generator/tags.hpp>

namespace
{
	namespace detail
	{
		template <class Element>
		auto join_list_elements(Element &&single)
		{
			return tags::li(std::forward<Element>(single));
		}

		template <class Element1, class Element2, class... ElementTail>
		auto join_list_elements(Element1 &&first, Element2 &&second,
		                      ElementTail &&... tail)
		{
			return tags::li(std::forward<Element1>(first)) +
                    join_list_elements(std::forward<Element2>(second),
			                        std::forward<ElementTail>(tail)...);
		}

	}

	template <class... Elements>
	auto unordered_list(Elements &&... cells)
	{
		return tags::ul(detail::join_list_elements(std::forward<Elements>(cells)...));
	}

	template <class... Elements>
	auto ordered_list(Elements &&... cells)
	{
		return tags::ol(
		    detail::join_list_elements(std::forward<Elements>(cells)...));
	}

    auto unordered_list()    {
        return tags::ul(Si::html::text(""));
    }

    auto ordered_list()    {
        return tags::ol(Si::html::text(""));
    }
}