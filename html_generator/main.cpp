#include <boost/program_options.hpp>
#include <iostream>
#include <ventura/open.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/read_file.hpp>
#include <silicium/sink/file_sink.hpp>
#include <silicium/sink/throwing_sink.hpp>
#include <silicium/html/tree.hpp>

namespace
{
	template <std::size_t N>
	auto anchor_attributes(char const(&name)[N])
	{
		return Si::html::attribute("name", name) + Si::html::attribute("href", std::string("#") + name);
	}

	template <class Element>
	auto p(Element &&content)
	{
		return Si::html::tag("p", std::forward<Element>(content));
	}

	template <class Element, class Attributes>
	auto p(Attributes &&attributes, Element &&content)
	{
		return Si::html::tag("p", std::forward<Attributes>(attributes), std::forward<Element>(content));
	}

	template <class Element>
	auto h1(Element &&content)
	{
		return Si::html::tag("h1", std::forward<Element>(content));
	}

	template <class Element>
	auto h2(Element &&content)
	{
		return Si::html::tag("h2", std::forward<Element>(content));
	}

	template <class Element>
	auto h3(Element &&content)
	{
		return Si::html::tag("h3", std::forward<Element>(content));
	}

	template <std::size_t N>
	auto link(std::string const &protocol, char const(&address_without_protocol)[N])
	{
		using namespace Si::html;
		return p(tag("a", attribute("href", protocol + address_without_protocol), text(address_without_protocol)));
	}

	template <class StringLike>
	auto make_code_snippet(StringLike const &code)
	{
		using namespace Si::html;
		std::size_t const lines = std::count(std::begin(code), std::end(code), '\n') + 1;
		std::string line_numbers;
		for (std::size_t i = 1; i <= lines; ++i)
		{
			line_numbers += boost::lexical_cast<std::string>(i);
			line_numbers += '\n';
		}
		return tag("div", attribute("style", "white-space:nowrap"),
		           tag("pre", attribute("style", "display:inline-block"), text(std::move(line_numbers))) +
		               tag("pre", attribute("style", "display:inline-block;margin-left:20px"), text(code)));
	}

	boost::system::error_code generate_all_html(ventura::absolute_path const &snippets_source_code,
	                                            ventura::absolute_path const &existing_output_root)
	{
		ventura::absolute_path const index_path = existing_output_root / ventura::relative_path("index.html");
		Si::error_or<Si::file_handle> const index =
		    ventura::overwrite_file(ventura::safe_c_str(to_os_string(index_path)));
		if (index.is_error())
		{
			std::cerr << "Could not overwrite file " << index_path << '\n';
			return index.error();
		}

		auto const snippet_from_file = [&snippets_source_code](char const *name)
		{
			ventura::absolute_path const full_name = snippets_source_code / ventura::relative_path(name);
			Si::variant<std::vector<char>, boost::system::error_code, ventura::read_file_problem> read_result =
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
				    boost::throw_exception(std::runtime_error("Could not read file " + to_utf8_string(full_name) +
				                                              ": " + boost::lexical_cast<std::string>(error)));
				},
			    [&full_name](ventura::read_file_problem const problem)
			    {
				    switch (problem)
				    {
				    case ventura::read_file_problem::file_too_large_for_memory:
					    boost::throw_exception(
					        std::runtime_error("File " + to_utf8_string(full_name) + " cannot be read into memory"));

				    case ventura::read_file_problem::concurrent_write_detected:
					    boost::throw_exception(
					        std::runtime_error("File " + to_utf8_string(full_name) +
					                           " cannot be read because it seems to be accessed concurrently"));
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
		};

		Si::file_sink index_sink(index.get().handle);
		using namespace Si::html;
		char const title[] = "TyRoXx' blog";

		auto articles = h2(text("Articles")) + text("Sorry, there are no finished articles yet.");

		auto throwing_constructor =
		    h3(tag("a", anchor_attributes("throwing-constructor"), text("Throwing from a constructor"))) +
		    snippet_from_file("throwing_constructor_0.cpp");
		auto drafts = h2(text("Drafts")) + std::move(throwing_constructor);

		auto todo = h2(text("Technical to do list")) + tag("ul", tag("li", text("compile the code snippets")) +
		                                                             tag("li", text("color the code snippets")) +
		                                                             tag("li", text("clang-format the code snippets")));

		auto links = link("https://", "github.com/TyRoXx") + link("https://", "twitter.com/tyroxxxx") +
		             link("mailto:", "tyroxxxx@gmail.com");
		auto style = "body {\n"
		             "	font-size: 16px;\n"
		             "	margin: 40px auto;\n"
		             "	max-width: 650px;\n"
		             "}\n"
		             "p {\n"
		             "	line-height: 1.6;\n"
		             "}\n";
		auto head = tag("head", tag("meta", attribute("charset", "utf-8"), empty) + tag("title", text(title)) +
		                            tag("style", text(style)));
		auto body =
		    tag("body", h1(text(title)) + std::move(links) + std::move(articles) + std::move(drafts) + std::move(todo));
		auto const document = raw("<!DOCTYPE html>") + tag("html", std::move(head) + std::move(body));
		auto erased_sink = Si::Sink<char, Si::success>::erase(Si::make_throwing_sink(index_sink));
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
	desc.add_options()("help", "produce help message")("output", boost::program_options::value(&output_option),
	                                                   "a directory to put the HTML files into");

	boost::program_options::positional_options_description positional;
	positional.add("output", 1);
	boost::program_options::variables_map vm;
	try
	{
		boost::program_options::store(
		    boost::program_options::command_line_parser(argc, argv).options(desc).positional(positional).run(), vm);
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

	Si::optional<ventura::absolute_path> const output_root = ventura::absolute_path::create(output_option);
	if (!output_root)
	{
		std::cerr << "The output directory must be given as an absolute path.\n";
		std::cerr << desc << "\n";
		return 1;
	}

	{
		boost::system::error_code const ec = ventura::create_directories(*output_root, Si::return_);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}

	{
		boost::system::error_code const ec =
		    generate_all_html(*ventura::parent(*ventura::parent(*ventura::absolute_path::create(__FILE__))) /
		                          ventura::relative_path("snippets"),
		                      *output_root);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}
}
