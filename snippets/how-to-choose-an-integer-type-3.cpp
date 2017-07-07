std::vector<char> c_str_to_vector(char const *c_str)
{
	// WTF Why does it say int when strlen returns size_t?
	// WTF What if the string is longer than numeric_limits<int>::max()?
	int length = std::strlen(c_str);
	// WTF Why is an int added to a pointer and not a ptrdiff_t?
	// WTF What if the int is negative?
	std::vector<char> v(c_str, c_str + length);
	return v;
}