#include "helper.h"

void changeColor(const char* color){
    //changes the color of the text printed in console
    printf(color);
}

void ERROR(bool _exit,const char* message){
    //a simple function to print systemic and non systemic errors
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

int min(int a,int b){return a>b?b:a;}//returns minimum of two numbers
char* nexPart(char* inp,char* end){
    /*if a string contains two parts separated by \0 it returns a pointer to the next part
    if the next part doesnt exist it returns NULL*/
    char* temp=inp+strlen(inp)+1;
    if(temp>=end){
        return NULL;
    }else{
        while(*temp==' '){
            temp++;
        }
        return temp; 
       

    }
   
}
