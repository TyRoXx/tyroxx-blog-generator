#pragma once

#include <string>
#include "cpp_syntax_highlighting.hpp"
#include "html_generator/snippets.h"

enum class markdown_types
{
	eof,
	text,
	inline_code
};

struct markdown_token
{
	std::string content;
	markdown_types type;
};

template <class RandomAccessIterator>
std::string find_next_paragraph(RandomAccessIterator begin,
                                RandomAccessIterator end)
{
	if (begin == end)
	{
		return "";
	}
	bool is_new_line = false;
	return std::string(begin,
	                   std::find_if(begin + 1, end, [&](char c)
	                                {
		                                if (is_new_line && is_line_end(c))
			                                return true;
		                                is_new_line = is_line_end(c);
		                                return false;
		                            }));
}

template <class RandomAccessIterator>
markdown_token find_next_mark_down_token(RandomAccessIterator &begin,
                                         RandomAccessIterator end)
{
	if (begin == end)
	{
		return {"", markdown_types::eof};
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
		                                        return c == '`';

		                                    })),
	        markdown_types::text};
}

auto compile_paragraph(std::string source)
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
			                         case markdown_types::inline_code:
				                         inline_code(token.content)
				                             .generate(sink);
				                         i += 1;
				                         break;
			                         case markdown_types::text:
				                         if (!(token.content.size() == 1 &&
				                               is_line_end(token.content[0])))
				                         {
					                         Si::html::text(token.content)
					                             .generate(sink);
				                         }
			                         }
			                         i += token.content.size();
		                         }
		                     });
}

auto compile(std::string source)
{
	return Si::html::dynamic([source =
	                              std::move(source)](Si::html::code_sink & sink)
	                         {
		                         auto i = source.begin();
		                         for (;;)
		                         {
			                         std::string paragraph =
			                             find_next_paragraph(i, source.end());
			                         if (paragraph.size() == 0)
			                         {
				                         return;
			                         }

			                         Si::html::tag("p",
			                                       compile_paragraph(paragraph))
			                             .generate(sink);
			                         i += paragraph.size();
		                         }

		                     });
}