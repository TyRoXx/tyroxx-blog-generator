#include <string>
struct widget
{
	explicit widget(std::string const &important_file)
	{
		std::ifstream file(important_file, std::ios::binary);
		important_data.insert(important_data.begin(),
		                      std::istreambuf_iterator<char>(file),
		                      std::istreambuf_iterator<char>());
		if (!file)
		{
			throw std::runtime_error("Could not read file " + important_file);
		}
	}

private:
	std::vector<char> important_data;
};
