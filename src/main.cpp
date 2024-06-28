#include <cxxopts.hpp>
#include <fmt/core.h>
#include <cpr/cpr.h>
#include <fmt/color.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


#include <string>
#include <string_view>

struct ProgramOptions {
  std::string url;
  int width;
};

ProgramOptions parse_cli(int argc, char* argv[]) {
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

  return ProgramOptions{
    .url = result["url"].as<std::string>(),
    .width = result["width"].as<int>()
  };
}

char luminance_to_ascii(float luminance) {
  static const std::string
    ASCII_CHARS = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

  size_t position = luminance * (ASCII_CHARS.size() - 1);
  return ASCII_CHARS[position];
}

cv::Mat download_image(std::string_view url) {
  cpr::Response r = cpr::Get(cpr::Url{url});

  if (r.status_code != 200) {

    throw std::runtime_error{fmt::format("Impossible to download {}. HTTP error {}", url, r.status_code)};
  }

  cv::Mat rawData( 1, r.text.size(), CV_8UC1, (void*)r.text.data());
  cv::Mat resultImage = cv::imdecode(rawData, cv::IMREAD_COLOR);

  return resultImage;
}

cv::Mat resize_image(cv::Mat image, int new_width) {
  int new_height = static_cast<int>(static_cast<double>(image.cols) / image.rows * new_width * .2);
  cv::Mat result;
  cv::resize(image, result, cv::Size{new_width, new_height}, 0.0, 0.0, cv::INTER_NEAREST);

  return result;
}

void print_ascii_art(cv::Mat image) {
  int nChannels = image.channels();
  int nRows = image.rows;
  int nCols = image.cols * nChannels;

  for(int i = 0; i < nRows; ++i) {
    auto row = image.ptr<uchar>(i);
    for(int j = 0; j < nCols; j += nChannels) {

      auto b = row[j + 0] / 255.0;
      auto g = row[j + 1] / 255.0;
      auto r = row[j + 2] / 255.0;

      float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;
      char ascii_char = luminance_to_ascii(luminance);
      fmt::print(fmt::fg(fmt::rgb(uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255))), "{}", ascii_char);
    }
    fmt::print("\n");
  }
}

int main(int argc, char* argv[]) {
  try {
    auto options = parse_cli(argc, argv);
    auto image = download_image(options.url);
    auto resized = resize_image(image, options.width);
    print_ascii_art(resized);  
    return 0;
  }
  catch(const std::exception& exc) {
    fmt::println("Something went wrong: {}", exc.what());
  }
  catch(...) {
    fmt::println("Something went really wrong");
  }
  
  return 1;
}
