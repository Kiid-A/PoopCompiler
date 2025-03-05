#include <iostream>

int main(int argc, char **argv)
{
    std::string filename;
    if (argc > 1) filename = argv[1];
    else printf("error:\n command: lexC filename");
    return 0;
}