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