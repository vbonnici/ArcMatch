#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "include/timer.h"
#include <stdio.h>
#include <stdlib.h>



class FileReader{
    FILE * pFile;
    long lSize;
    char * buffer;

    long pi;

public:
    FileReader (const char * filename){
        size_t result;

        pFile = fopen ( filename , "rb" );
        if (pFile==NULL) {fputs ("File error\n",stderr); exit (1);}

        // obtain file size:
        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);

        // allocate memory to contain the whole file:
        buffer = (char*) malloc (sizeof(char)*lSize);
        if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}

        // copy the file into the buffer:
        result = fread (buffer,1,lSize,pFile);
        if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}

        /* the whole file is now loaded in the memory buffer. */

        pi = 0;

        //std::cout<<buffer<<"\n";
    }

    bool
    is_valid(){
        return pi<lSize;
    }


    const char *
    next_string(){
        
        while((pi < lSize) && ((buffer[pi]==' ')||(buffer[pi]=='\n')||(buffer[pi]=='\t')||(buffer[pi]=='\r') )){
            pi++;
        }

        long ci = pi;
        
        while((pi < lSize) && (buffer[pi]!=' ')&&(buffer[pi]!='\n')&&(buffer[pi]!='\t')&&(buffer[pi]!='\r') ){
            pi++;
        }

        if(pi < lSize){
            buffer[pi] = '\0';
        }
        pi++;

        //std::cout<<"ci "<<ci<<"; pi "<<pi<<"\n";

        return buffer+ci;
    }

    int
    next_int(){

        while((pi < lSize) && ((buffer[pi]==' ')||(buffer[pi]=='\n')||(buffer[pi]=='\t')||(buffer[pi]=='\r') )){
            pi++;
        }

        int ret = 0;
        
        while((pi < lSize) && (buffer[pi]!=' ')&&(buffer[pi]!='\n')&&(buffer[pi]!='\t')&&(buffer[pi]!='\r') ){
            ret = (ret*10) + (buffer[pi]-'0');

            pi++;
        }

        return ret;
    }


    void close(){
        fclose (pFile);
        free (buffer);
    }


};





int main(int argc, char* argv[]){
    TIMEHANDLE time_total_s = start_time();

    std::cout<<argv[1]<<"\n";

    FileReader fire(argv[1]);

    while(fire.is_valid()){
        //std::cout<<fire.next_string()<<"\n";
        std::cout<<fire.next_int()<<"\n";
    }


    fire.close();


    std::cout<<"total time "<<end_time(time_total_s)<<"\n";
};