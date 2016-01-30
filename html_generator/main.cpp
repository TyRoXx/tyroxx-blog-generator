#include <boost/program_options.hpp>
#include <iostream>
#include <ventura/open.hpp>
#include <ventura/file_operations.hpp>
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

	template <std::size_t N>
	auto make_code_snippet(char const(&code)[N])
	{
		using namespace Si::html;
		std::size_t const lines = std::count(std::begin(code), std::end(code), '\n') + 1;
		std::string line_numbers;
		for (std::size_t i = 1; i <= lines; ++i)
		{
			line_numbers += boost::lexical_cast<std::string>(i);
			line_numbers += '\n';
		}
		return tag("pre", attribute("style", "float:left"), text(std::move(line_numbers))) +
		       tag("pre", attribute("style", "float:left;margin-left:30px"), text(code));
	}

	boost::system::error_code generate_all_html(ventura::absolute_path const &existing_root)
	{
		ventura::absolute_path const index_path = existing_root / ventura::relative_path("index.html");
		Si::error_or<Si::file_handle> const index =
		    ventura::overwrite_file(ventura::safe_c_str(to_os_string(index_path)));
		if (index.is_error())
		{
			std::cerr << "Could not overwrite file " << index_path << '\n';
			return index.error();
		}

		Si::file_sink index_sink(index.get().handle);
		using namespace Si::html;
		char const title[] = "TyRoXx' blog";
		auto articles = h2(text("Articles")) + text("Sorry, there are no finished articles yet.");
		auto four_spaces =
		    h3(tag("a", anchor_attributes("one-tab-is-four-spaces"), text("One tab is four spaces"))) +
		    p(text("The following program demonstrates that one tab is in fact equivalent to four spaces:")) +
		    make_code_snippet("#include <iostream>\n"
		                      "#define tab\n"
		                      "int main()\n"
		                      "{\n"
		                      "tab std::cout << \"one tab \"\n"
		                      "    std::cout << \"is four spaces\\n\";\n"
		                      "}") +
		    p(attribute("style", "clear:left"),
		      text("Both ways achieve exactly the same result: Indenting by four characters. Only the characters "
		           "used for indentation are different. The old argument has "
		           "finally been settled."));
		auto drafts = h2(text("Drafts")) + std::move(four_spaces);
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
		auto body = tag("body", h1(text(title)) + std::move(links) + std::move(articles) + std::move(drafts));
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
		boost::system::error_code const ec = generate_all_html(*output_root);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}
}
