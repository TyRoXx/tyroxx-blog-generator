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
	auto make_anchor_attributes(char const(&name)[N])
	{
		return Si::html::attribute("name", name) + Si::html::attribute("href", std::string("#") + name);
	}

	boost::system::error_code generate_all_html(ventura::absolute_path const &existing_root)
	{
		ventura::absolute_path const index_path = existing_root / ventura::relative_path("index.html");
		Si::error_or<Si::file_handle> const index = ventura::overwrite_file(ventura::safe_c_str(index_path));
		if (index.is_error())
		{
			std::cerr << "Could not overwrite file " << index_path << '\n';
			return index.error();
		}

		Si::file_sink index_sink(index.get().handle);
		using namespace Si::html;
		char const title[] = "TyRoXx' blog";
		auto drafts = tag("h2", text("Drafts")) +
		              tag("h3", tag("a", make_anchor_attributes("how-to-use-travis-ci-org-for-cpp"),
		                            text("How to use travis-ci.org for C++"))) +
		              tag("p", text("......"));
		auto articles = tag("h2", text("Articles"));
		auto const document =
		    tag("html", tag("head", tag("title", text(title))) +
		                    tag("body", tag("h1", text(title)) + std::move(drafts) + std::move(articles)));
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
