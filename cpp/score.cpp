#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

using namespace std;

int main(void)
{
        string name;

        cout << "Enter your first name: ";
        cin >> name;
        cout << "Enter your last name: ";

        if (string tmp; cin >> tmp)
                name += " " + tmp;

        cout << "Hi, " + name + ".\n";

        cout << "Enter your midterm and final exam grades: \n";

        double midterm, final;

        cin >> midterm >> final;

        cout << "Enter all your homework grades, "
                "followed by end-of-file: ";

        int cnt = 0;
        double sum = 0;
        double x;

        while (cin >> x) {
                ++cnt;
                sum += x;
        }

        streamsize prec = cout.precision();

        cout << "Your final grade is " << setprecision(3)
                << 0.2 * midterm + 0.4 * final + 0.4 * sum / cnt
                << setprecision(prec) << endl;

        return 0;
}
