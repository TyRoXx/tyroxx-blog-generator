#include "../html_generator/blog_lang.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(render_something) {
    std::string html_generated;
    auto erased_html_sink = Si::Sink<char, Si::success>::erase(
            Si::make_container_sink(html_generated));
    auto tree = compile(s);
    tree.generate(erased_html_sink);
    std::cout << html_generated;
}
