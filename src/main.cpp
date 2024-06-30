#include <cpr/cpr.h>
#include <cxxopts.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <optional>
#include <string>

struct ProgramOptions {
  std::string url;
  int width;
};

struct DownloadedImageAndOptions {
  cv::Mat image;
  const ProgramOptions &options;
};

std::optional<ProgramOptions> parse_cli(int argc, char *argv[]);
std::optional<DownloadedImageAndOptions> download_image(const ProgramOptions &url);
cv::Mat resize_image(const DownloadedImageAndOptions &imageAndOptions);
std::string to_ascii_art(cv::Mat image);

int main(int argc, char *argv[]) {
  // clang-format off
  const auto ascii_art =
      parse_cli(argc, argv)
        .and_then(download_image)
        .transform(resize_image)
        .transform(to_ascii_art)
        .value_or("Sorry, something went wrong!")
      ;
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
      return {};
    }

    return ProgramOptions{.url = result["url"].as<std::string>(), .width = result["width"].as<int>()};
  } catch (...) {
    return {};
  }
}

std::optional<DownloadedImageAndOptions> download_image(const ProgramOptions &options) {
  cpr::Response r = cpr::Get(cpr::Url{options.url});

  if (r.status_code != 200) {
    return {};
  }

  cv::Mat rawData(1, r.text.size(), CV_8UC1, (void *)r.text.data());
  cv::Mat resultImage = cv::imdecode(rawData, cv::IMREAD_COLOR);

  return DownloadedImageAndOptions{resultImage, options};
}

cv::Mat resize_image(const DownloadedImageAndOptions &imageAndOptions) {
  const auto &image = imageAndOptions.image;
  const auto new_width = imageAndOptions.options.width;

  int new_height = static_cast<int>(static_cast<double>(image.cols) / image.rows * new_width * .2);
  cv::Mat result;
  cv::resize(image, result, cv::Size{new_width, new_height}, 0.0, 0.0, cv::INTER_LINEAR);

  return result;
}

char luminance_to_ascii(float luminance) {
  // clang-format off
  static const std::string ASCII_CHARS = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
  // clang-format on

  size_t position = luminance * (ASCII_CHARS.size() - 1);
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

      const auto b = row_view[j + 0] / 255.0;
      const auto g = row_view[j + 1] / 255.0;
      const auto r = row_view[j + 2] / 255.0;

      const float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;
      const char ascii_char = luminance_to_ascii(luminance);
      result.append(fmt::format(fmt::fg(fmt::rgb(uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255))), "{}", ascii_char));
    }
    result.append("\n");
  }
  return result;
}

