#pragma once
#include "tags.hpp"
#include <silicium/html/tree.hpp>

inline bool is_brace(char const c)
{
	static boost::string_ref const braces = "(){}<>[]";
	return std::find(braces.begin(), braces.end(), c) != braces.end();
}

enum class token_type
{
	identifier,
	string,
	double_colon,
	preprocessor,
	brace,
	space,
	eof,
	other
};

struct token
{
	std::string content;
	token_type type;
};

template <class RandomAccessIterator>
token find_next_token(RandomAccessIterator begin, RandomAccessIterator end)
{
	if (begin == end)
	{
		return {"", token_type::eof};
	}
	// Detecting identifier
	if (isalnum(*begin))
	{
		return {std::string(begin, std::find_if(begin + 1, end,
		                                        [](char c) {
			                                        return !isalnum(c) &&
			                                               c != '_';
			                                    })),
		        token_type::identifier};
	}
	// Detecting name spaces
	if (*begin == ':')
	{
		if (begin + 1 == end)
		{
			return {":", token_type::other};
		}
		if (begin[1] != ':')
		{
			return {std::string(begin, begin + 2), token_type::other};
		}
		return {"::", token_type::double_colon};
	}
	// Detecting whitespaces
	if (!isprint(*begin))
	{
		return {
		    std::string(begin, std::find_if(begin + 1, end,
		                                    [](char c) { return isprint(c); })),
		    token_type::space};
	}
	// Detecting braces
	if (is_brace(*begin))
	{
		return {std::string(begin, begin + 1), token_type::brace};
	}
	// Detecting escaped characters
	if (*begin == '"' || *begin == '\'')
	{
		char first = *begin;
		bool escaped = false;
		RandomAccessIterator end_index =
		    std::find_if(begin + 1, end, [&escaped, first](char c) {
			    if (escaped)
			    {
				    escaped = false;
				    return false;
			    }
			    if (c == '\\')
			    {
				    escaped = true;
				    return false;
			    }
			    return (c == first);
			});
		if (end_index == end)
		{
			throw std::invalid_argument("Number of quotes must be even");
		}
		return {std::string(begin, end_index + 1), token_type::string};
	}
	// Detecting pre processor directives
	if (*begin == '#')
	{
		return {std::string(begin, std::find_if(begin + 1, end,
		                                        [](char c) {
			                                        return c == '\n' ||
			                                               c == '\r' ||
			                                               c == '"';
			                                    })),
		        token_type::preprocessor};
	}
	return {std::string(begin, std::find_if(begin + 1, end,
	                                        [](char c) {
		                                        return isalnum(c) || c == '"' ||
		                                               c == ':' || c == '\'';
		                                    })),
	        token_type::other};
}

inline auto render_code_raw(std::string code)
{
	using namespace Si::html;
	return dynamic([code = std::move(code)](code_sink & sink) {
		static const boost::string_ref keywords[] = {
		    "alignas",      "alignof",
		    "and",          "and_eq",
		    "asm",          "auto",
		    "bitand",       "bitor",
		    "bool",         "break",
		    "case",         "catch",
		    "char",         "char16_t",
		    "char32_t",     "class",
		    "compl",        "const",
		    "constexpr",    "const_cast",
		    "continue",     "decltype",
		    "default",      "delete",
		    "do",           "double",
		    "dynamic_cast", "else",
		    "enum",         "explicit",
		    "export",       "extern",
		    "false",        "float",
		    "for",          "friend",
		    "goto",         "if",
		    "inline",       "int",
		    "long",         "mutable",
		    "namespace",    "new",
		    "noexcept",     "not",
		    "not_eq",       "nullptr",
		    "operator",     "or",
		    "or_eq",        "private",
		    "protected",    "public",
		    "register",     "reinterpret_cast",
		    "return",       "short",
		    "signed",       "sizeof",
		    "static",       "static_assert",
		    "static_cast",  "struct",
		    "switch",       "template",
		    "this",         "thread_local",
		    "throw",        "true",
		    "try",          "typedef",
		    "typeid",       "typename",
		    "union",        "unsigned",
		    "using",        "virtual",
		    "void",         "volatile",
		    "wchar_t",      "while",
		    "xor",          "xor_eq",
		    "override",     "final"};
		auto i = code.begin();
		for (;;)
		{
			token t = find_next_token(i, code.end());
		token_switch:
			switch (t.type)
			{
			case token_type::eof:
				return;

			case token_type::preprocessor:
				tags::span(attribute("class", "preprocessor"), text(t.content))
				    .generate(sink);
				break;

			case token_type::string:
				tags::span(attribute("class", "stringLiteral"), text(t.content))
				    .generate(sink);
				break;

			               case token_type::identifier:
				               if (std::find(std::begin(keywords),
				                             std::end(keywords),
				                             t.content) != std::end(keywords))
				               {
					               tags::span(attribute("class", "keyword"),
					                          text(t.content))
					                   .generate(sink);
					               break;
				               }
				               else
				               {
					               i += t.content.size();
					               token next = find_next_token(i, code.end());
					               if (next.type == token_type::double_colon)
					               {
						               tags::span(attribute("class", "names"),
						                          text(t.content))
						                   .generate(sink);
					               }
					               else
					               {
						               text(t.content).generate(sink);
					               }
					               t = next;
					               goto token_switch;
				               }

			case token_type::double_colon:
				while (t.type == token_type::identifier ||
				       t.type == token_type::double_colon)
				{
					tags::span(attribute("class", "names"), text(t.content))
					    .generate(sink);
					i += t.content.size();
					t = find_next_token(i, code.end());
				}
				goto token_switch;

			case token_type::space:
			case token_type::other:
			case token_type::brace:
				text(t.content).generate(sink);
			}
			i += t.content.size();
		}
	});
			               case token_type::space:
			               case token_type::other:
			               case token_type::brace:
				               text(t.content).generate(sink);
				               break;
			               }
			               i += t.content.size();
		               }
		           });
}

inline auto render_code(std::string code)
{
	using namespace Si::html;
	return tag("code", render_code_raw(std::move(code)));
}
