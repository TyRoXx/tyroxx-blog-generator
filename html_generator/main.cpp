#include <boost/program_options.hpp>
#include <iostream>
#include <ventura/open.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/read_file.hpp>
#include <silicium/sink/file_sink.hpp>
#include <silicium/sink/throwing_sink.hpp>
#include <silicium/html/tree.hpp>
#include "tags.hpp"

namespace
{
	enum class token_type
	{
		identifier,
		string,
		names,
		eof,
		other
	};

	struct token
	{
		std::string content;
		token_type type;
	};

	template <class InputIterator>
	token find_next_token(InputIterator begin, InputIterator end)
	{
		if (begin == end)
		{
			return {"", token_type::eof};
		}
		if (isalnum(*begin))
		{
			std::string content =
			    std::string(begin, std::find_if(begin + 1, end, [](char c)
			                                    {
				                                    return !isalnum(c) &&
				                                           c != '_' && c != ':';
				                                }));
			auto colonCount = std::count(content.begin(), content.end(), ':');
			if (colonCount == 0)
			{
				return {content, token_type::identifier};
			}
			if (colonCount % 2 == 0)
			{
				return {content, token_type::names};
			}
			return {content, token_type::other};
		}
		if (*begin == '"' || *begin == '\'')
		{
			char first = *begin;
			bool escaped = false;
			InputIterator endIndex =
			    std::find_if(begin + 1, end, [&escaped, first](char c)
			                 {
				                 if (c == '\\')
				                 {
					                 escaped = true;
				                 }
				                 if (!escaped)
				                 {
					                 return c == first;
				                 }
				                 else
				                 {
					                 escaped = false;
				                 }
				                 return true;
				             });
			if (endIndex == end)
			{
				throw std::invalid_argument("Number of quotes must be even");
			}
			return {std::string(begin, endIndex + 1), token_type::string};
		}
		return {std::string(begin, std::find_if(begin + 1, end,
		                                        [](char c)
		                                        {
			                                        return isalnum(c) ||
			                                               c == '"' ||
			                                               c == '\'';
			                                    })),
		        token_type::other};
	}

	template <class StringLike>
	auto make_code_snippet(StringLike const &code)
	{
		using namespace Si::html;
		std::size_t const lines =
		    std::count(std::begin(code), std::end(code), '\n') + 1;
		std::string line_numbers;
		for (std::size_t i = 1; i <= lines; ++i)
		{
			line_numbers += boost::lexical_cast<std::string>(i);
			line_numbers += '\n';
		}
		auto codeTag = tag(
		    "code",
		    dynamic([code](code_sink &sink)
		            {
			            static const std::string keywords[] = {
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
				            case token_type::other:
					            text(t.content).generate(sink);
					            break;
				            case token_type::string:
					            span(attribute("class", "stringLiteral"),
					                 text(t.content))
					                .generate(sink);
					            break;
				            case token_type::identifier:
					            if (std::find(std::begin(keywords),
					                          std::end(keywords),
					                          t.content) != std::end(keywords))
					            {
						            span(attribute("class", "keyword"),
						                 text(t.content))
						                .generate(sink);
					            }
					            else
					            {
						            text(t.content).generate(sink);
					            }
					            break;
				            case token_type::names:
					            span(attribute("class", "names"),
					                 text(t.content))
					                .generate(sink);
				            }
				            i += t.content.size();
			            }
			        }));

		return div(cl("sourcecodeSnippet"),
		           pre(cl("lineNumbers"), text(std::move(line_numbers))) +
		               pre(cl("code"), codeTag) + br());
	}

	auto snippet_from_file(ventura::absolute_path const &snippets_source_code,
	                       char const *name)
	{
		ventura::absolute_path const full_name =
		    snippets_source_code / ventura::relative_path(name);
		Si::variant<std::vector<char>, boost::system::error_code,
		            ventura::read_file_problem> read_result =
		    ventura::read_file(ventura::safe_c_str(to_os_string(full_name)));
		std::vector<char> content;
		Si::visit<void>(
		    read_result,
		    [&content](std::vector<char> &read_content)
		    {
			    content = std::move(read_content);
			},
		    [&full_name](boost::system::error_code const error)
		    {
			    boost::throw_exception(std::runtime_error(
			        "Could not read file " + to_utf8_string(full_name) + ": " +
			        boost::lexical_cast<std::string>(error)));
			},
		    [&full_name](ventura::read_file_problem const problem)
		    {
			    switch (problem)
			    {
			    case ventura::read_file_problem::file_too_large_for_memory:
				    boost::throw_exception(
				        std::runtime_error("File " + to_utf8_string(full_name) +
				                           " cannot be read into memory"));

			    case ventura::read_file_problem::concurrent_write_detected:
				    boost::throw_exception(std::runtime_error(
				        "File " + to_utf8_string(full_name) +
				        " cannot be read because it seems to be accessed "
				        "concurrently"));
			    }
			});

		std::string clean;
		for (char c : content)
		{
			switch (c)
			{
			case '\t':
				clean += "    ";
				break;

			case '\r':
				break;

			default:
				clean += c;
				break;
			}
		}
		return make_code_snippet(clean);
	}

	boost::system::error_code
	generate_all_html(ventura::absolute_path const &snippets_source_code,
	                  ventura::absolute_path const &existing_output_root,
	                  std::string const fileName)
	{
		ventura::absolute_path const index_path =
		    existing_output_root / ventura::relative_path(fileName);
		Si::error_or<Si::file_handle> const index = ventura::overwrite_file(
		    ventura::safe_c_str(to_os_string(index_path)));
		if (index.is_error())
		{
			std::cerr << "Could not overwrite file " << index_path << '\n';
			return index.error();
		}

		Si::file_sink index_sink(index.get().handle);
		using namespace Si::html;
		std::string siteTitle;
		if (fileName == "index.html")
		{
			siteTitle = "TyRoXx' blog";
		}
		else
		{
			siteTitle = "TyRoXx' contacts";
		}
		auto articles =
		    h2("Articles") + p("Sorry, there are no finished articles yet.");

		auto drafts = h2("Drafts") +
#include "pages/input-validation.hpp"
		              +
#include "pages/throwing-constructor.hpp"
		    ;

		auto menu = dynamic(
		    [&fileName](code_sink &sink)
		    {
			    if (fileName == "contact.html")
			    {
				    tag("menu",
				        ul(li(link("https://", "github.com/TyRoXx")) +
				           li(link("https://", "twitter.com/tyroxxxx")) +
				           li(link("mailto:", "tyroxxxx@gmail.com")) +
				           li(link("", "index.html", "Back"))))
				        .generate(sink);
			    }
			    else
			    {
				    tag("menu",
				        ul(li(tag("a", attribute("href", "contact.html"),
				                  text("Contacts")))))
				        .generate(sink);
			    }
			});

		auto todo =
		    h2("Technical to do list") +
		    ul(li("compile the code snippets") + li("color the code snippets") +
		       li("clang-format the code snippets"));

		auto headContent =
		    head(tag("meta", attribute("charset", "utf-8"), empty) +
		         title(siteTitle) +
		         tag("link",
		             href("stylesheets.css") + attribute("rel", "stylesheet"),
		             empty));
		auto bodyContent =
		    body(std::move(menu) + h1(siteTitle) + std::move(articles) +
		         std::move(drafts) + std::move(todo));
		auto const document =
		    raw("<!DOCTYPE html>") +
		    html(std::move(headContent) + std::move(bodyContent));
		auto erased_sink = Si::Sink<char, Si::success>::erase(
		    Si::make_throwing_sink(index_sink));
		try
		{
			document.generate(erased_sink);
			return {};
		}
		catch (boost::system::system_error const &ex)
		{
			// TODO: do this without an exception
			return ex.code();
		}
	}
}

int main(int argc, char **argv)
{
	std::string output_option;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")(
	    "output", boost::program_options::value(&output_option),
	    "a directory to put the HTML files into");

	boost::program_options::positional_options_description positional;
	positional.add("output", 1);
	boost::program_options::variables_map vm;
	try
	{
		boost::program_options::store(
		    boost::program_options::command_line_parser(argc, argv)
		        .options(desc)
		        .positional(positional)
		        .run(),
		    vm);
	}
	catch (boost::program_options::error const &ex)
	{
		std::cerr << ex.what() << '\n' << desc << "\n";
		return 1;
	}

	boost::program_options::notify(vm);

	if (vm.count("help"))
	{
		std::cerr << desc << "\n";
		return 1;
	}

	if (output_option.empty())
	{
		std::cerr << "Please provide an absolute path to generate to.\n";
		std::cerr << desc << "\n";
		return 1;
	}

	Si::optional<ventura::absolute_path> const output_root =
	    ventura::absolute_path::create(output_option);
	if (!output_root)
	{
		std::cerr
		    << "The output directory must be given as an absolute path.\n";
		std::cerr << desc << "\n";
		return 1;
	}

	{
		boost::system::error_code const ec =
		    ventura::create_directories(*output_root, Si::return_);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}

	{
		ventura::absolute_path repo = *ventura::parent(
		    *ventura::parent(*ventura::absolute_path::create(__FILE__)));
		boost::system::error_code const ec =
		    generate_all_html(repo / ventura::relative_path("snippets"),
		                      *output_root, "index.html");
		boost::system::error_code const ec2 =
		    generate_all_html(repo / ventura::relative_path("snippets"),
		                      *output_root, "contact.html");
		ventura::copy(repo / ventura::relative_path(
		                         "html_generator/pages/stylesheet.css"),
		              *output_root / ventura::relative_path("stylesheets.css"),
		              Si::return_);

		if (!!ec && !!ec2)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}
}
