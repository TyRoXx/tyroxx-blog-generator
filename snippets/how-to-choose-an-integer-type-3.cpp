std::vector<char> c_str_to_vector(char const *c_str)
{
	// WTF Why does it say int when strlen returns size_t?
	// WTF What if the string is longer than numeric_limits<int>::max()?
	int length = std::strlen(c_str);
	// WTF Why is an int passed as an argument to a constructor that takes
	// size_t?
	// WTF What if the int is negative?
	// WTF Will this fail safely with bad_alloc or will worse things happen?
	std::vector<char> v(c_str, length);
	return v;
}