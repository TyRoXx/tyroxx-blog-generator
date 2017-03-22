#include <iostream>
#include <silicium/sink/ostream_sink.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>

int main()
{
	try
	{
		ventura::absolute_path const here =
		    ventura::get_current_working_directory(Si::throw_);
		ventura::absolute_path const generator =
		    here / ventura::relative_path("html_generator.exe");
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back(to_utf8_string(here));
		auto output =
		    Si::Sink<char, Si::success>::erase(Si::ostream_ref_sink(std::cout));
		int const rc =
		    ventura::run_process(generator, arguments, here, output, {},
		                         ventura::environment_inheritance::inherit)
		        .get();
		if (rc != 0)
		{
			std::cerr << generator << " " << arguments[0] << " failed with "
			          << rc << '\n';
			return 1;
		}
	}
	catch (boost::system::system_error const &ex)
	{
		std::cerr << ex.what() << '\n';
		return 1;
	}
}
