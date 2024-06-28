#include <cxxopts.hpp>
#include <fmt/core.h>
#include <cpr/cpr.h>
#include <fmt/color.h>
#include <stb_image.h>
#include <stb_image_resize2.h>

#include <string>
#include <string_view>

struct ProgramOptions {
  std::string url;
  int width;
};

class StbImage {
public:
  explicit StbImage(const stbi_uc* data, std::size_t size) {
    fmt::println("DEBUG: StbImage({}, {})", (const void*)data, size);
      m_data = stbi_load_from_memory(
      data,
      size,
      &m_width,
      &m_height,
      &m_channels,
      0
    );

    if (m_data == nullptr) {
      throw std::runtime_error{"Unable to convert the image"};
    }
  }

  StbImage(const StbImage&) = delete;
  StbImage operator=(const StbImage&) = delete;

  ~StbImage() {
    stbi_image_free(m_data);
  }

  int width() const noexcept { return m_width; }
  int height() const noexcept { return m_height; }
  int channels() const noexcept {return m_channels; }
  const unsigned char* data() const noexcept { return m_data; }

  unsigned char operator[](size_t index) const noexcept { return m_data[index]; }

private:
  unsigned char* m_data;
  int m_width;
  int m_height;
  int m_channels;
};

char luminance_to_ascii(float luminance) {
  static const std::string
    ASCII_CHARS = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

  size_t position = luminance * (ASCII_CHARS.size() - 1);
  return ASCII_CHARS[position];
}

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

std::string download_image(std::string_view url) {
  cpr::Response r = cpr::Get(cpr::Url{url});

  if (r.status_code != 200) {

    throw std::runtime_error{fmt::format("Impossible to download {}. HTTP error {}", url, r.status_code)};
  }

  fmt::println("The image size is {} bytes", r.text.size());
  return r.text;
}

StbImage load_image(std::string_view imageData) {
  return StbImage{
    reinterpret_cast<const stbi_uc*>(imageData.data()),
    imageData.size()
  };
}

StbImage resize_image(const StbImage& image, int new_width) {
  // Adjust aspect ratio for ASCII art
  int new_height = static_cast<int>(static_cast<double>(image.height()) / image.width() * new_width * 0.45);
  int new_channels = image.channels();
  auto new_buffer = (unsigned char*)malloc(new_width * new_height * new_channels);
  
  fmt::println("DEBUG: before resizing, new_height: {}", new_height);

  auto newImageData = stbir_resize_uint8_linear(
    image.data(), image.width(), image.height(), 0, 
    new_buffer, new_width, new_height, 0,
    stbir_pixel_layout::STBIR_RGB 
  );

  if (newImageData == nullptr) {
    throw std::runtime_error{"newImage is null"};
  }

  fmt::println("DEBUG: The new image pointer is {}");

  return StbImage(newImageData, new_width * new_height * 3);
}

void print_image(const StbImage& image, int new_width) {
  // Adjust aspect ratio for ASCII art
  int new_height = static_cast<int>(static_cast<double>(image.height()) / image.width() * new_width * 0.45);

  for (int i = 0; i < new_height; ++i) {
    for (int j = 0; j < new_width; ++j) {
      int old_i = i * image.height() / new_height;
      int old_j = j * image.width() / new_width;

      float r = image[(old_i * image.width() + old_j) * image.channels() + 0] / 255.0f;
      float g = image[(old_i * image.width() + old_j) * image.channels() + 1] / 255.0f;
      float b = image[(old_i * image.width() + old_j) * image.channels() + 2] / 255.0f;
      float luminance = (0.2126f * r + 0.7152f * g + 0.0722f * b);

      char ascii_char = luminance_to_ascii(luminance);

      // Use fmt to print ASCII character with color
      fmt::print(fmt::fg(fmt::rgb(uint8_t(r * 255), uint8_t(g * 255), uint8_t(b * 255))), "{}", ascii_char);
    }
    fmt::print("\n");
  }
}

int main(int argc, char* argv[]) {
  try {
    auto options = parse_cli(argc, argv);
    auto imageData = download_image(options.url);
    auto image = load_image(imageData);
    // auto resizedImage = resize_image(image, options.width);
    print_image(image, options.width);
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
