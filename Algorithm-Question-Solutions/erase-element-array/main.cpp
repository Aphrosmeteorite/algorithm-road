#include "EraseElemArray.h"
#include <iostream>

int main(int argc, char** argv) {
    int arr[10] = {0};
    int size = 10;
    for( int i=0; i<size; i++){
        std::cin >> arr[i];
    }
    erase(5, arr, size);
    for( int i=0; i<size; i++){
        std::cout << arr[i] << ' ';
    } // 1 2 3 4 7 8 9 10
    std::cout << '\n';
}