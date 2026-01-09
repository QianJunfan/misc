#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct score_data {
        string name;
        int score;
};

int main(void)
{
        vector<score_data> scores = {
                {"Alice", 95},
                {"Bob", 95},
                {"Charlie", 88},
                {"David", 55},
        };

        cout << "Score:" << endl;

        for (const auto& s : scores)
                cout << s.name << ": " << s.score << endl;

        for (auto& s : scores) {
                if (s.score < 60) {
                        s.score += 5;
                        cout << "\nadd 5 points for " << s.name << endl;
                }
        }

        cout << "\nFinal:" << endl;

        for (const auto& [name, score] : scores)
                cout << "Name: " << name << " | Final Score: " << score << endl;
                
              
        return 0;
}
