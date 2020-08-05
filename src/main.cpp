#include <iostream>
#include <string_view>
#include <vector>

#define VERSION "0.1" // TODO: This should be set in configuration

int main(int argc, char *argv[])
{
    std::vector<std::string_view> opts(argv,argv+argc);
}
