#include "helper.h"

void changeColor(const char* color){
    //changes the color of text that is going to be printed in console
    printf(color);
}

void ERROR(bool _exit,const char* message){
    //a simple function to print errors
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

int min(int a,int b){return a>b?b:a;}
