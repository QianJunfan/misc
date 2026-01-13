#include <iostream>
#include <string>

using namespace std;

int main(void)
{
        cout << "input: ";
        string val;
        cin >> val;

        const string str = "a new" + val + "string";

        const string spaces(str.size(), 'a');

        cout << str << endl;
        cout << spaces << endl;
        
        return 0;        
}
