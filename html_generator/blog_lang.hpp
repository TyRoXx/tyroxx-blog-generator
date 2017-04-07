#include <string>
#include <ventura/relative_path.hpp>
#include <ventura/absolute_path.hpp>
#include "cpp_syntax_highlighting.hpp"
#include "snippets.h"
#include "tags.hpp"

ventura::absolute_path repo = *ventura::parent(
        *ventura::parent(*ventura::absolute_path::create(__FILE__)));
ventura::absolute_path snippets_source_code = repo / ventura::relative_path("snippets");

std::string s = "# How to choose an integer type\n"
        "(created 2017-04-05, updated 2017-04-05)\n"
        "Prefer portable types like `std::uint32_t` over the types without an explicit range. Use `int`, `long`, `long long`"
        "only if you have a reason to use exactly these and not the portable ones. C++ is different from Java and C#: `int` etc are not the same size and range on every platform. This makes "
        "them hard to use even for C++ experts. They are neither necessary nor sufficient for most practical situations.\n"
        "In most cases you already know the expected range of values. So why not use the right integer type for this instead instead of "
        "cargo-culting it?\n"
        "If you know that you are dealing with 64-bit file sizes, document "
        "this assumption in an unambiguous way. There is no reason for "
        "using `int` or `long long` in this case. Using the wrong type leads to unportable code and "
        "misunderstandings.";
/*
                snippet_from_file(snippets_source_code, ventura::relative_path("how-to-choose-an-integer-type-0.cpp")) +
                "One of the few valid use cases for the built-in types is overloading a function for all integer types that exist:" +
                snippet_from_file(snippets_source_code, ventura::relative_path("how-to-choose-an-integer-type-1.cpp")) +
                "This kind of code should be fairly uncommon though.\nThe other major kind of integer types are the pointery ones: " +
                "`std::size_t`, `std::ptrdiff_t` and `std::uintptr_t`. They are used when dealing with memory on the current machine. " +
                "`int` cannot store the sizes of objects on most contemporary machines. `size_t` can. Use `size_t`. `ptrdiff_t` is for" +
                " differences between memory addresses. It is signed because a difference can be negative. `uintptr_t` is for manipulating pointers on the bit "
                        "level. If you did not know about `uintptr_t` before reading this article, chances are you won't need this type any soon. " +
                "`size_t` and `ptrdiff_t` are two of the most important types in C++ you have to know and make use of." +
                " This is a very small function how it would be written by someone who does not waste a single thought on choosing the right type." +
                snippet_from_file(snippets_source_code, ventura::relative_path("how-to-choose-an-integer-type-2.cpp")) +
                "These are my thoughts when I read this function. I tried to mark the WTF moments so that you can calculate the WTFs per line metric as a take-home exercise if you like." +
                snippet_from_file(snippets_source_code, ventura::relative_path("how-to-choose-an-integer-type-3.cpp")) +
                "I only have to change one little thing to make all these WTFs go away:" +
                snippet_from_file(snippets_source_code, ventura::relative_path("how-to-choose-an-integer-type-4.cpp")) +
                "Much better, isn't it? Is it really too much to ask for using the right integer types?";
*/
enum class markdown_types {
    eof,
    text,
    heading,
    inline_code,
    snippet
};

struct markdown_token {
    std::string content;
    markdown_types type;
};

template<class RandomAccessIterator>
markdown_token find_next_mark_down_token(RandomAccessIterator& begin, RandomAccessIterator end) {
    if (begin == end) {
        return {"", markdown_types::eof};
    }
    if (*begin == '#') {
        begin += 1;
        return {std::string(begin, std::find_if(begin + 1, end, is_line_end)), markdown_types::heading};
    }
    if (*begin == '`') {
        begin += 1;
        return {std::string(begin, std::find_if(begin + 1, end,
                                                    [](char c) {
                                                        return c == '`';
                                                    })), markdown_types::inline_code};
    }
    return {std::string(begin, std::find_if(begin + 1, end,
                                            [](char c) {
                                                return c == '`' || is_line_end(c);
                                            })),
            markdown_types::text};
}

auto compile(std::string source) {
    return Si::html::dynamic([source = std::move(source)](Si::html::code_sink &sink) {
        auto i = source.begin();
        for (;;) {
            markdown_token token = find_next_mark_down_token(i, source.end());
            switch (token.type) {
                case markdown_types::eof:
                    return;
                case markdown_types::heading:
                    tags::h1(Si::html::text(token.content)).generate(sink);
                    break;
                case markdown_types::inline_code:
                    inline_code(token.content).generate(sink);
                    i+=1;
                    break;
                case markdown_types::snippet:
                    snippet_from_file(snippets_source_code, ventura::relative_path(token.content));
                    break;
                case markdown_types::text:
                    if(! is_line_end(token.content[0])){
                        tags::p(token.content).generate(sink);
                    }
            }
            i += token.content.size();
        }
    });
}