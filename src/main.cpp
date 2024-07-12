#include <cpr/cpr.h>
#include <cxxopts.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <optional>
#include <string>
#include <string_view>

struct ProgramOptions {
  std::string url;
  int width;
};

std::optional<ProgramOptions> parse_cli(int argc, char *argv[]);
std::optional<cv::Mat> download_and_resize_image(ProgramOptions options);
std::string to_ascii_art(cv::Mat image);

int main(int argc, char *argv[]) {
  // clang-format off
  const auto ascii_art =
      parse_cli(argc, argv)
        .and_then(download_and_resize_image)
        .transform(to_ascii_art)
        .value_or("Sorry, something went wrong!");
  // clang-format on

  fmt::print("{}", ascii_art);
  return 0;
}

std::optional<ProgramOptions> parse_cli(int argc, char *argv[]) {
  try {
    cxxopts::Options options("img2ascii", "Convert images to ASCII art");
    // clang-format off
    options.add_options()
      (
        "h,help", "Help"
      )
      (
        "u,url",
        "URL of the image to convert to ASCII art",
        cxxopts::value<std::string>()
          ->default_value("https://cppusergroupvienna.org/wp-content/uploads/2024/05/cpp_usergroup_vienna_bright_300dpi.png")
      )
      (
        "w,width",
        "Width in characters of the output image",
        cxxopts::value<int>()->default_value("80")
      );
    // clang-format on

    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      fmt::println("{}", options.help());
      exit(0);
    }

    return ProgramOptions{.url = result["url"].as<std::string>(), .width = result["width"].as<int>()};
  } catch (...) {
    return std::nullopt;
  }
}

cv::Mat resize_image(cv::Mat image, int new_width) {
  const auto width_factor = new_width * 0.15;

  int new_height = static_cast<int>(
      static_cast<double>(image.cols) / image.rows * width_factor);

  cv::Mat result;
  const double fx = 0.0, fy = 0.0;
  cv::resize(
      image, result,
      cv::Size{new_width, new_height},
      fx, fy,
      cv::INTER_LINEAR
  );

  return result;
}

std::optional<cv::Mat> download_and_resize_image(ProgramOptions options) {
  cpr::Response r = cpr::Get(cpr::Url{options.url});

  if (r.status_code != 200) {
    return std::nullopt;
  }

  cv::Mat raw_data(1, size(r.text), CV_8UC1, std::data(r.text));
  cv::Mat image = cv::imdecode(raw_data, cv::IMREAD_COLOR);

  return resize_image(image, options.width);
}

char luminance_to_ascii(std::floating_point auto luminance) {
  // clang-format off
  constexpr std::string_view ASCII_CHARS =
    " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neo"
    "Z5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
  // clang-format on

  const auto position = static_cast<size_t>(
      std::round(
          luminance * (ASCII_CHARS.size() - 1))
      );
  return ASCII_CHARS[position];
}

std::string to_ascii_art(cv::Mat image) {
  std::string result;

  int channels = image.channels();
  int rows = image.rows;
  int cols = image.cols * channels;

  for (int i = 0; i < rows; ++i) {
    const auto row_view = image.ptr<uchar>(i);
    for (int j = 0; j < cols; j += channels) {
      const auto b = row_view[j + 0];
      const auto g = row_view[j + 1];
      const auto r = row_view[j + 2];

      const auto bf = b / 255.0;
      const auto gf = g / 255.0;
      const auto rf = r / 255.0;

      const auto luminance = 0.2126 * rf + 0.7152 * gf + 0.0722 * bf;
      const char ascii_char = luminance_to_ascii(luminance);

      result.append(fmt::format(fmt::fg(fmt::rgb(r, g, b)), "{}", ascii_char));
    } // End of col
    result.append("\n");
  }
  return result;
}

