#include "../html_generator/blog_lang.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE(render_something) {
    std::string output = compile(s);
    std::cout << output;
}
