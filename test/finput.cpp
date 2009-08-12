#include "iostream.h"
#include "fstream.h"

int main() {
    ifstream in("data.dat");

    if(!in) {
        cout << "Cannot open file.\n";
        return 1;
    }

    char item[20];
    float cost;

    cout.precision(2);

    int >> item >> cost;
    cout<<item<<""<<cost<<"\n"

    in.close();
    return 0;
}
