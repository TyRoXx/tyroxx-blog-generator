std::vector<char> c_str_to_vector(char const *c_str)
{
	std::size_t length = std::strlen(c_str);
	std::vector<char> v(c_str, length);
	return v;
}