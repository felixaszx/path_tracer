#include <iostream>
#include <string>
#include <vector>

#include "frame.hpp"

namespace Program
{
    class Program
    {
      public:
        int main(int argc, char** argv)
        {
            Frame ff(1920, 1080, 4);




            ff.to_png("result.png");
            return EXIT_SUCCESS;
        }
    };

} // namespace Program