#include <cxxopts.hpp>
#include <fmt/core.h>
#include <cpr/cpr.h>

#include <string>

int main(int argc, char* argv[]) {

  cxxopts::Options options("img2ascii", "Convert images to ASCII art");

  options.add_options()
     ("h,help", "Help")
     ("u,url", "URL of the image to convert to ASCII art", cxxopts::value<std::string>()->default_value("https://cppusergroupvienna.org/wp-content/uploads/2024/05/cpp_usergroup_vienna_bright_300dpi.png"))
     ("w,width", "Width in characters of the output image", cxxopts::value<int>()->default_value("80"));

  auto result = options.parse(argc, argv);
  if(result.count("help")) {
    fmt::println("{}", options.help());
    exit(0);
  }

  cpr::Response r = cpr::Get(cpr::Url{result["url"].as<std::string>()});

  fmt::println("Status Code: {}", r.status_code);

  return 0;
}
