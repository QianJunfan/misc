#include <iostream>

int main(void)
{
        std::cout << "Enter tow numbers:" << std::endl;
        int a = 0;
        int b = 0;

        std::cin >> a >> b;
        std::cout << "The sum of " << a << " and " << b
                  << " is " << a + b << std::endl;

        return 0;
}
