#include <iostream>
#include <vector>
#include "testVector.h"

// 动态数组（Vector）


void testVector(){
    // 创建一个空的整数向量
    std::vector<int> myVector;

    // 添加元素到向量中
    myVector.push_back(3);
    myVector.push_back(7);
    myVector.push_back(11);
    myVector.push_back(5);

    // 访问向量中的元素并输出
    std::cout << "Elements in the vector: ";
    for (int element : myVector) {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    // 删除向量中的第三个元素
    myVector.erase(myVector.begin() + 2);

    // // 清空向量并输出
    // myVector.clear();
    // // 清空向量并输出
    // myVector.clear();

}



void testVector_MAIN(){

     testVector();
}
