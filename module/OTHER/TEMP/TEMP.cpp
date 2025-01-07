#include <iostream>
#include <stdio.h>
#include "TEMP.h"


TEMP::TEMP_Class::TEMP_Class() {
    // 可以在这里进行初始化操作
    std::cout << "TEMPger initialized." << std::endl;
}

// TEMP 方法的实现
void TEMP::TEMP_Class::TEMP_info() {
    std::cout << "TEMP_info: "<< std::endl;
}


void TEMP_init(){
    std::cout << "TEMP initialized." << std::endl;
}


int TEMP_MAIN(){
    
}
