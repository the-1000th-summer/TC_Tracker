#include <ctime>
#include <iostream>
#include <chrono>
#include "Typhoon.h"
#include "Utils.h"

using namespace std;

int main(int, char**) {
    time_t timep = 0;
    auto a = gmtime(&timep);
    cout << asctime(a) << endl;
    // Typhoon a;
    // a.sayHello();
    
    // const std::wstring bb = L"2014-07-25T20:17:22Z";
    auto aa = UtilFunc::getEpochTime(L"1970-01-01T00:00:00Z");
    cout << aa << endl;
    // cout << UtilFunc::ifFileExists("const std::string &name") << endl;

    // cout << asctime(&aa) << endl;
    // cout << ctime(&timep) << endl;
}
