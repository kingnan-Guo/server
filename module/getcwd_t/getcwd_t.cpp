#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
void getw(){
    char path[256];
    getcwd(path, 256);
    cout << "getcwd() = " << path << endl;
    char * name = get_current_dir_name();
    cout << "name = " << name << endl;

    free(name);
}


// ./sylar /opt/
int direbtTest(int arg, char* argv[]){
    if(arg != 2){
        return -1;
    }

    DIR * dir;

    if( (dir = opendir(argv[1])) == 0){
        return -1;
    }



    struct dirent *stdinfo = nullptr;// 用于存放目录中对去到的内容

    while(1){

        if((stdinfo = readdir(dir)) == nullptr){
            break;
        }

        cout << "文件名  == " << stdinfo->d_name << "文件类型 == " << (int)stdinfo->d_type << endl;
    }
    closedir(dir);


}

int testError(){
    int iret = mkdir("/tmp/aaa", 0755); // 文件 已经存在 会报错
    cout << "iret = " << iret << endl;
    cout << errno << ":" << strerror(errno) << endl;


    perror("调用 mkdir(/tmp/aaa/bb/cc/dd) fail ");
    return 0;

}


int accessTest(int arg, char* argv[]){
    // 判断文件是否存在
    if(access(argv[1], F_OK) != 0){
        cout << "目录" << argv[1] << "文件不存在 \n" << endl;
        return -1;
    } 
        cout << "目录" << argv[1] << "文件存在 \n" << endl;

    return 0;

}


void getcwd_main(int arg, char* argv[]){
    getw();

    direbtTest(arg, argv);

    testError();

    accessTest( arg, argv);
}