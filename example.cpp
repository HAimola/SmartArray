#include "SmartArray.h"

int main(){
    SmartArray<int>a(3);
    a.from_arr<3>({1, 2, 3});

    std::cout << a << "\n";
    a.pop();

    for(auto i: SmartArrayReverse<int>(a)){
        std::cout << i;
    }
    
    a.put(500);

    std::cout << "\n" << a << "\n"; 
    std::cout << a[3]; // ERROR: out-of-bounds

}