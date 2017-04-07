#include <boost/lexical_cast.hpp>
#include <silicium/variant.hpp>
#include <ventura/read_file.hpp>

auto inline_code(std::string code)
{
    return tags::span(tags::cl("inlineCodeSnippet"),
                      render_code(std::move(code)));
}

template <class StringLike>
auto make_code_snippet(StringLike const &code)
{
    using namespace Si::html;
    std::size_t const lines =
            std::count(std::begin(code), std::end(code), '\n') + 1;
    std::string line_numbers;
    for (std::size_t i = 1; i <= lines; ++i)
    {
        line_numbers += boost::lexical_cast<std::string>(i);
        line_numbers += '\n';
    }
    auto code_tag = render_code(code);
    return tags::div(
            tags::cl("sourcecodeSnippet"),
            tags::pre(tags::cl("lineNumbers"), text(std::move(line_numbers))) +
            tags::pre(std::move(code_tag)));
}

auto snippet_from_file(ventura::absolute_path const &snippets_source_code,
                       ventura::relative_path const &name)
{
    ventura::absolute_path const full_name = snippets_source_code / name;
    Si::variant<std::vector<char>, boost::system::error_code,
    ventura::read_file_problem> read_result =
            ventura::read_file(ventura::safe_c_str(to_os_string(full_name)));
    std::vector<char> content;
    Si::visit<void>(
            read_result,
            [&content](std::vector<char> &read_content)
            {
                content = std::move(read_content);
            },
            [&full_name](boost::system::error_code const error)
            {
                boost::throw_exception(std::runtime_error(
                        "Could not read file " + to_utf8_string(full_name) + ": " +
                        boost::lexical_cast<std::string>(error)));
            },
            [&full_name](ventura::read_file_problem const problem)
            {
                switch (problem)
                {
                    case ventura::read_file_problem::file_too_large_for_memory:
                        boost::throw_exception(
                                std::runtime_error("File " + to_utf8_string(full_name) +
                                                   " cannot be read into memory"));

                    case ventura::read_file_problem::concurrent_write_detected:
                        boost::throw_exception(std::runtime_error(
                                "File " + to_utf8_string(full_name) +
                                " cannot be read because it seems to be accessed "
                                        "concurrently"));
                }
            });

    std::string clean;
    for (char c : content)
    {
        switch (c)
        {
            case '\t':
                clean += "    ";
                break;

            case '\r':
                break;

            default:
                clean += c;
                break;
        }
    }
    return make_code_snippet(std::move(clean));
}