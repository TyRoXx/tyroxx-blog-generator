#pragma once
#include <string>
#include "cpp_syntax_highlighting.hpp"
#include "tags.hpp"
#include "snippets.h"

enum class markdown_types
{
	eof,
	text,
	heading,
	inline_code
};

struct markdown_token
{
	std::string content;
	markdown_types type;
};

template <class RandomAccessIterator>
markdown_token find_next_mark_down_token(RandomAccessIterator &begin,
                                         RandomAccessIterator end)
{
	if (begin == end)
	{
		return {"", markdown_types::eof};
	}
	if (*begin == '#')
	{
		begin += 1;
		return {std::string(begin, std::find_if(begin + 1, end, is_line_end)),
		        markdown_types::heading};
	}
	if (*begin == '`')
	{
		begin += 1;
		return {std::string(begin, std::find_if(begin + 1, end,
		                                        [](char c)
		                                        {
			                                        return c == '`';
			                                    })),
		        markdown_types::inline_code};
	}
	return {std::string(begin, std::find_if(begin + 1, end,
	                                        [](char c)
	                                        {
		                                        return c == '`' ||
		                                               is_line_end(c);
		                                    })),
	        markdown_types::text};
}

auto compile(std::string source)
{
	return Si::html::dynamic([source =
	                              std::move(source)](Si::html::code_sink & sink)
	                         {
		                         auto i = source.begin();
		                         for (;;)
		                         {
			                         markdown_token token =
			                             find_next_mark_down_token(
			                                 i, source.end());
			                         switch (token.type)
			                         {
			                         case markdown_types::eof:
				                         return;
			                         case markdown_types::heading:
				                         tags::h1(Si::html::text(token.content))
				                             .generate(sink);
				                         break;
			                         case markdown_types::inline_code:
				                         inline_code(token.content)
				                             .generate(sink);
				                         i += 1;
				                         break;
			                         case markdown_types::text:
				                         if (!is_line_end(token.content[0]))
				                         {
					                         tags::p(token.content)
					                             .generate(sink);
				                         }
			                         }
			                         i += token.content.size();
		                         }
		                     });
}