#include <iostream>

void ccd()
{
    std::cout << rand();
}

int main(int argc, char** argv)
{
    srand(time(nullptr));
    for (int i = 0; i < 3000; i++)
    {
        ccd();
    }
}