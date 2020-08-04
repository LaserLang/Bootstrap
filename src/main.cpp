#include <iostream>

#define VERSION "0.1" // TODO: This should be set in configuration

int main(int argc, char *argv[])
{
    std::cout << "laserc bootstrap version " << VERSION << std::endl << std::endl;
    std::cout << "Executable: " << argv[0] << std::endl;
    std::cout << "Parameters: ";
    for (int i = 1; i < argc; i++)
    {
        std::cout << argv[i];
        if (i < argc - 1)
            std::cout << ", ";
        else
            std::cout << std::endl;
    }
}
