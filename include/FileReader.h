/*
Copyright (c) 2023

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

ArcMatch is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
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