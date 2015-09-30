//
//  utils.h
//  Doxygen Plugin
//
//  Created by Iheb Eddine BADRANI on 29/09/2015.
//  Copyright (c) 2015 Iheb Eddine BADRANI. All rights reserved.
//

#ifndef Doxygen_Plugin_utils_h
#define Doxygen_Plugin_utils_h


#include <iostream>
#include <fstream>
#include <vector>


struct FunctionInfo_Struct
{
    char * FunctionName;
    char FunctionId[100];
    
    FunctionInfo_Struct() : FunctionName(NULL)
    {}
    
    ~FunctionInfo_Struct()
    {
        if (FunctionName)
        {
            delete [] FunctionName;
            FunctionName = NULL;
        }
    }
};


void ReadFileContentIntoString(const char * fileName, std::string & content);
bool GetExtractFunctionsInfo(const char * fileName, std::vector<FunctionInfo_Struct> & FunctionList);
bool ExtractFunctionInfo(const char * xFunctionName, const char * xFunctionLink, FunctionInfo_Struct & functionInfo);

#endif
