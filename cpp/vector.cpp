#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(void)
{
        vector<int> a;
        vector<int> b = {1, 2, 3};
        vector<int> c(5, 100);
        vector<int> d(b.begin(), b.end());

        a.push_back(1);
        a.push_back(2);
        a.emplace_back(3);

        // O(n) - low performance
        a.insert(a.begin(), 0);

        a.at(2) = 20; // safe

        int first = a.front();
        int last  = a.back();

        a.pop_back();
        a.erase(a.begin());
        // a.clear();

        cout << "size: " << a.size() << "\n";
        cout << "capa: " << a.capacity() << "\n";

        cout << "ent: ";
        for (const auto& x : b)
                cout << x << " ";

        sort(b.begin(), b.end());
        reverse(b.begin(), b.end());

        auto it = find(b.begin(), b.end(), 3);

        if (it != b.end())
                cout << "\n Found 3 at idx: " << distance(b.begin(), it);

        vector<vector<int>> matrix(3, vector<int>(4, 0));
        matrix[1][2] = 99;
        
        return 0;
}
