#include "helper.h"

void changeColor(const char* color){
    printf(color);
}

void ERROR(bool _exit,const char* message){
    char temp[200];
     if(message!=NULL){  
        strcpy(temp,RED);
        strcat(temp,message);
        perror(temp);
    }else{
        strcpy(temp,RED);
        strcat(temp,"ERROR");
        perror(temp);
    }
    changeColor(RESET);
    if(_exit){
        exit(1);//exiting with non zero valu
    }

}