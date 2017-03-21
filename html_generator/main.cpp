#include <boost/program_options.hpp>
#include <iostream>
#include <ventura/open.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/read_file.hpp>
#include <silicium/sink/file_sink.hpp>
#include <silicium/sink/throwing_sink.hpp>
#include <silicium/html/tree.hpp>
#include "tags.hpp"
#include <beast/http/read.hpp>
#include <beast/http/string_body.hpp>
#include <beast/http/write.hpp>
#include <beast/core/streambuf.hpp>
#include <ventura/read_file.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "cpp_syntax_highlighting.hpp"

namespace
{
	auto inline_code(std::string code)
	{
		return tags::div(tags::cl("inlineCodeSnippet"),
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

	boost::system::error_code
	generate_all_html(ventura::absolute_path snippets_source_code,
	                  ventura::absolute_path const &existing_output_root,
	                  boost::string_ref const file_name)
	{
		ventura::absolute_path const index_path =
		    existing_output_root /
		    ventura::relative_path(file_name.begin(), file_name.end());
		Si::error_or<Si::file_handle> const index = ventura::overwrite_file(
		    ventura::safe_c_str(to_os_string(index_path)));
		if (index.is_error())
		{
			std::cerr << "Could not overwrite file " << index_path << '\n';
			return index.error();
		}

		Si::file_sink index_sink(index.get().handle);
		using namespace Si::html;

		std::string site_title;
		if (file_name == "index.html")
		{
			site_title = "TyRoXx' blog";
		}
		else if (file_name == "contact.html")
		{
			site_title = "TyRoXx' contacts";
		}
		else if (file_name == "articles.html")
		{
			site_title = "List of articles";
		}
		else
		{
			site_title = "Unknown page";
		}

		auto page_content = dynamic(
		    [
			  &file_name,
			  snippets_source_code = std::move(snippets_source_code)
			](code_sink & sink)
		    {
			    if (file_name == "index.html")
			    {
				    auto drafts = tags::h2(text("Drafts")) +
#include "pages/input-validation.hpp"
				                  +
#include "pages/throwing-constructor.hpp"
				        ;
				    drafts.generate(sink);
			    }
			    if (file_name == "articles.html")
			    {
				    tags::p("Sorry, there are no finished articles yet.")
				        .generate(sink);
			    }
			    if (file_name == "contact.html")
			    {
				    tags::h2(text("Technical to do list")).generate(sink);
				    tags::ul(tags::li(text("compile the code snippets")) +
				             tags::li(text("[done] color the code snippets")) +
				             tags::li(text("clang-format the code snippets")))
				        .generate(sink);
			    }
			});

		auto head_content = tags::head(
		    tag("meta", attribute("charset", "utf-8"), empty) +
		    tag("meta",
		        attribute("name", "viewport") +
		            attribute("content", "width=device-width, initial-scale=1"),
		        empty) +
		    tags::title(site_title) +
		    tag("link",
		        tags::href("stylesheets.css") + attribute("rel", "stylesheet"),
		        empty));
		auto body_content = tags::body(
#include "pages/menu.hpp"
		    +tags::h1(text(site_title)) + std::move(page_content) +
#include "pages/footer.hpp"
		    );
		auto const document =
		    raw("<!DOCTYPE html>") +
		    tags::html(std::move(head_content) + std::move(body_content));
		auto erased_sink = Si::Sink<char, Si::success>::erase(
		    Si::make_throwing_sink(index_sink));
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

	struct file_client
	{
		boost::asio::ip::tcp::socket socket;
		beast::streambuf receive_buffer;
		beast::http::response<beast::http::string_body> response;

		explicit file_client(boost::asio::ip::tcp::socket socket,
		                     beast::streambuf receive_buffer)
		    : socket(std::move(socket))
		    , receive_buffer(std::move(receive_buffer))
		{
		}
	};

	struct http_client
	{
		boost::asio::ip::tcp::socket socket;
		beast::streambuf receive_buffer;
		beast::http::request<beast::http::string_body> request;

		explicit http_client(boost::asio::ip::tcp::socket socket,
		                     beast::streambuf receive_buffer)
		    : socket(std::move(socket))
		    , receive_buffer(std::move(receive_buffer))
		{
		}
	};

	void begin_serve(std::shared_ptr<http_client> client,
	                 ventura::absolute_path const &document_root);

	void serve_prepared_response(
	    std::shared_ptr<file_client> client, bool const is_keep_alive,
	    ventura::absolute_path const &document_root,
	    boost::optional<boost::string_ref> const content_type)
	{
		client->response.version = 11;
		client->response.fields.insert(
		    "Content-Length",
		    boost::lexical_cast<std::string>(client->response.body.size()));
		if (content_type)
		{
			client->response.fields.insert("Content-Type", *content_type);
		}
		beast::http::async_write(
		    client->socket, client->response,
		    [client, is_keep_alive,
		     document_root](boost::system::error_code const ec)
		    {
			    Si::throw_if_error(ec);
			    if (is_keep_alive)
			    {
				    auto http_client_again = std::make_shared<http_client>(
				        std::move(client->socket),
				        std::move(client->receive_buffer));
				    begin_serve(http_client_again, document_root);
			    }
			    else
			    {
				    client->socket.shutdown(
				        boost::asio::ip::tcp::socket::shutdown_both);
			    }
			});
	}

	void serve_static_file(std::shared_ptr<file_client> client,
	                       bool const is_keep_alive,
	                       ventura::absolute_path const &document_root,
	                       ventura::absolute_path const &served_document)
	{
		Si::visit<void>(
		    ventura::read_file(
		        ventura::safe_c_str(ventura::to_os_string(served_document))),
		    [&](std::vector<char> content)
		    {
			    // TODO: avoid the copy of the content
			    client->response.body.assign(content.begin(), content.end());
			    client->response.reason = "OK";
			    client->response.status = 200;
			},
		    [&](boost::system::error_code const ec)
		    {
			    std::cerr << "Could not read file " << served_document << ": "
			              << ec << '\n';
			    client->response.reason = "Internal Server Error";
			    client->response.status = 500;
			    client->response.body = client->response.reason;
			},
		    [&](ventura::read_file_problem const problem)
		    {
			    std::cerr << "Could not read file " << served_document
			              << " due to problem " << static_cast<int>(problem)
			              << '\n';
			    client->response.reason = "Internal Server Error";
			    client->response.status = 500;
			    client->response.body = client->response.reason;
			});
		serve_prepared_response(client, is_keep_alive, document_root,
		                        boost::none);
	}

	void begin_serve(std::shared_ptr<http_client> client,
	                 ventura::absolute_path const &document_root)
	{
		beast::http::async_read(
		    client->socket, client->receive_buffer, client->request,
		    [client, document_root](boost::system::error_code const ec)
		    {
			    Si::throw_if_error(ec);
			    std::shared_ptr<file_client> const new_client =
			        std::make_shared<file_client>(
			            std::move(client->socket),
			            std::move(client->receive_buffer));
			    if (!client->request.url.empty() &&
			        (client->request.url.front() == '/'))
			    {
				    boost::filesystem::path requested_file(
				        client->request.url.begin() + 1,
				        client->request.url.end());
				    if (requested_file.empty())
				    {
					    requested_file = "index.html";
				    }
				    if (requested_file.is_relative())
				    {
					    serve_static_file(
					        new_client,
					        beast::http::is_keep_alive(client->request),
					        document_root,
					        document_root / ventura::relative_path(
					                            std::move(requested_file)));
					    return;
				    }
			    }
			    new_client->response.reason = "Bad Request";
			    new_client->response.status = 400;
			    new_client->response.body = new_client->response.reason;
			    serve_prepared_response(
			        new_client, beast::http::is_keep_alive(client->request),
			        document_root, boost::string_ref("text/html"));
			});
	}

	void begin_accept(boost::asio::ip::tcp::acceptor &acceptor,
	                  ventura::absolute_path const &document_root)
	{
		auto client = std::make_shared<http_client>(
		    boost::asio::ip::tcp::socket(acceptor.get_io_service()),
		    beast::streambuf());
		acceptor.async_accept(client->socket,
		                      [&acceptor, client, document_root](
		                          boost::system::error_code const ec)
		                      {
			                      Si::throw_if_error(ec);
			                      begin_accept(acceptor, document_root);
			                      begin_serve(client, document_root);
			                  });
	}
}

int main(int argc, char **argv)
{
	std::string output_option;
	boost::uint16_t web_server_port = 0;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")(
	    "output", boost::program_options::value(&output_option),
	    "a directory to put the HTML files into")(
	    "serve", boost::program_options::value(&web_server_port),
	    "serve the output directory on this port");

	boost::program_options::positional_options_description positional;
	positional.add("output", 1);
	boost::program_options::variables_map vm;
	try
	{
		boost::program_options::store(
		    boost::program_options::command_line_parser(argc, argv)
		        .options(desc)
		        .positional(positional)
		        .run(),
		    vm);
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

	if (output_option.empty())
	{
		std::cerr << "Please provide an absolute path to generate to.\n";
		std::cerr << desc << "\n";
		return 1;
	}

	Si::optional<ventura::absolute_path> const output_root =
	    ventura::absolute_path::create(output_option);
	if (!output_root)
	{
		std::cerr
		    << "The output directory must be given as an absolute path.\n";
		std::cerr << desc << "\n";
		return 1;
	}

	{
		boost::system::error_code const ec =
		    ventura::create_directories(*output_root, Si::return_);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}

	ventura::absolute_path repo = *ventura::parent(
	    *ventura::parent(*ventura::absolute_path::create(__FILE__)));
	ventura::copy(
	    repo / ventura::relative_path("html_generator/pages/stylesheet.css"),
	    *output_root / ventura::relative_path("stylesheets.css"), Si::return_);
	static const boost::string_ref files_to_generate[] = {
	    "index.html", "contact.html", "articles.html"};
	for (boost::string_ref const file : files_to_generate)
	{
		boost::system::error_code const ec = generate_all_html(
		    repo / ventura::relative_path("snippets"), *output_root, file);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
	}
	{
		// Directory listing for articles
		boost::system::error_code ec;
		ventura::absolute_path articles =
		    repo / ventura::relative_path("html_generator/articles");
		boost::filesystem::directory_iterator itter =
		    boost::filesystem::directory_iterator(articles.to_boost_path(), ec);
		if (!!ec)
		{
			std::cerr << ec << '\n';
			return 1;
		}
		while (itter != end(itter))
		{
			const boost::filesystem::directory_entry file = *itter;
			std::cout << file.path().filename() << '\n';
			itter.increment(ec);
		}
	}

	if (vm.count("serve"))
	{
		using namespace boost::asio;

		io_service io;

		ip::tcp::acceptor acceptor_v6(
		    io, ip::tcp::endpoint(ip::tcp::v6(), web_server_port), true);
		acceptor_v6.listen();
		begin_accept(acceptor_v6, *output_root);

		ip::tcp::acceptor acceptor_v4(
		    io, ip::tcp::endpoint(ip::tcp::v4(), web_server_port), true);
		acceptor_v4.listen();
		begin_accept(acceptor_v4, *output_root);

		while (!io.stopped())
		{
			try
			{
				io.run();
			}
			catch (boost::system::system_error const &ex)
			{
				std::cerr << "boost::system::system_error: " << ex.code()
				          << '\n';
				io.reset();
			}
			catch (std::exception const &ex)
			{
				std::cerr << "std::exception: " << ex.what() << '\n';
				io.reset();
			}
		}
	}
}
