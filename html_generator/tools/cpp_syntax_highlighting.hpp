#pragma once

#include "html_generator/tags.hpp"
#include <string>

inline bool is_brace(char const c)
{
	static boost::string_ref const braces = "(){}<>[]";
	return std::find(braces.begin(), braces.end(), c) != braces.end();
}

inline bool is_line_end(char const c)
{
	return c == '\n' || c == '\r';
}

enum class token_type
{
	identifier,
	string,
	double_colon,
	preprocessor,
	comment,
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
	if (isalnum(*begin) || *begin == ':')
	{
		bool hasColon = (*begin == ':');
		size_t colonCount = (*begin == ':') ? 1 : 0;
		std::string content = std::string(
		    begin, std::find_if(begin + 1, end, [&](char c)
		                        {
			                        if (c == ':')
			                        {
				                        hasColon = true;
				                        colonCount++;
			                        }
			                        else if (colonCount % 2 != 0)
			                        {
				                        return true;
			                        }
			                        return !isalnum(c) && c != '_' && c != ':';
			                    }));
		if (!hasColon)
		{
			return {content, token_type::identifier};
		}
		if (colonCount % 2 == 0)
		{
			return {content, token_type::double_colon};
		}
		return {content, token_type::other};
	}
	// Detecting whitespaces
	if (!isprint(*begin))
	{
		return {std::string(begin, std::find_if(begin + 1, end,
		                                        [](char c)
		                                        {
			                                        return isprint(c);
			                                    })),
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
		    std::find_if(begin + 1, end, [&escaped, first](char c)
		                 {
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
		                                        [](char c)
		                                        {
			                                        return is_line_end(c) ||
			                                               c == '"';
			                                    })),
		        token_type::preprocessor};
	}
	// Detecting comments
	if (*begin == '/' && begin + 1 != end)
	{
		char comment_type = *(begin + 1);
		// Single line comments
		if (comment_type == '/')
		{
			return {
			    std::string(begin, std::find_if(begin + 1, end, is_line_end)),
			    token_type::comment};
		}
		// Multiple line comments
		if (comment_type == '*')
		{
			bool is_end = true;
			return {std::string(begin, std::find_if(begin + 1, end,
			                                        [&](char c)
			                                        {
				                                        if (is_end && c == '/')
				                                        {
					                                        return true;
				                                        }
				                                        is_end = (c == '*');
				                                        return false;
				                                    }) +
			                               1),
			        token_type::comment};
		}
	}
	return {std::string(begin, std::find_if(begin + 1, end,
	                                        [](char c)
	                                        {
		                                        return isalnum(c) || c == '"' ||
		                                               c == ':' || c == '\'' ||
		                                               c == '/';
		                                    })),
	        token_type::other};
}

inline auto render_code_raw(std::string code)
{
	using namespace Si::html;
	return dynamic([code = std::move(code)](code_sink & sink)
	               {
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
			               switch (t.type)
			               {
			               case token_type::eof:
				               return;

			               case token_type::preprocessor:
				               tags::span(attribute("class", "preprocessor"),
				                          text(t.content))
				                   .generate(sink);
				               break;

			               case token_type::comment:
				               tags::span(attribute("class", "comment"),
				                          text(t.content))
				                   .generate(sink);
				               break;

			               case token_type::string:
				               tags::span(attribute("class", "stringLiteral"),
				                          text(t.content))
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
				               }
				               else
				               {
					               text(t.content).generate(sink);
				               }
				               break;

			               case token_type::double_colon:
				               tags::span(attribute("class", "names"),
				                          text(t.content))
				                   .generate(sink);
				               break;
			               case token_type::space:
			               case token_type::other:
			               case token_type::brace:
				               text(t.content).generate(sink);
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
