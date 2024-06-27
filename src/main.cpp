#include "img2ascii.h"
#include <vector>
#include <string>

int main() {
    img2ascii();

    std::vector<std::string> vec;
    vec.push_back("test_package");

    img2ascii_print_vector(vec);
}
