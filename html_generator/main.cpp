#include <boost/program_options.hpp>
#include <iostream>
#include <ventura/open.hpp>
#include <ventura/file_operations.hpp>
#include <silicium/sink/file_sink.hpp>

namespace
{
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
		return Si::append(index_sink,
		                  "<html><head><title>TyRoXx' blog</title></head><body><h1>Hello, world!</h1></body></html>");
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
