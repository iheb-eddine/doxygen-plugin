//
//  main.cpp
//  Doxygen Plugin
//
//  Created by Iheb Eddine BADRANI on 29/09/2015.
//  Copyright (c) 2015 Iheb Eddine BADRANI. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>

#include "tinyxml.h"

#include "utils.h"

using namespace std;


int main(int argc, const char * argv[])
{
    // insert code here...
    cout << "Hello, World!\n";


    std::vector<FunctionInfo_Struct> FunctionList;

    GetExtractFunctionsInfo("/Users/ihebeddine/projects/doxygen/SamllExample/html/globals_func.html", FunctionList);
    
    
    return 0;
}


void ReadFileContentIntoString(const char * fileName, std::string & content)
{
    ifstream ifs(fileName);
    content.assign(istreambuf_iterator<char>(ifs) , istreambuf_iterator<char>());
}

bool GetExtractFunctionsInfo(const char * fileName, std::vector<FunctionInfo_Struct> & FunctionList)
{
    string globalsFuncContent;

    FunctionList.clear();

    ReadFileContentIntoString(fileName, globalsFuncContent);
    
    cout << globalsFuncContent << endl;
    
    TiXmlDocument doc;
    const char * strError = doc.Parse(globalsFuncContent.c_str());
    TiXmlElement * RootElement, * tempElement, * tempElement2;
    FunctionInfo_Struct tempFunctionInfo;
    
    if (strError == NULL)
    {
        std::cout << "Could not parse the file " << globalsFuncContent << "\n";
        return false;
    }

    RootElement = doc.RootElement();

    if (strcmp(RootElement->Value(), "html"))
    {
        cout << "GetExtractFunctionsInfo : Could not get the tag html" << endl;
        return false;
    }


    tempElement = RootElement->FirstChildElement("body");
    
    if (! tempElement)
    {
        cout << "GetExtractFunctionsInfo : Could not get the tag body" << endl;
        return false;
    }
    
    
    tempElement = tempElement->FirstChildElement("div");
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement())
    {
        if (tempElement->Attribute("class") && ! strcmp(tempElement->Attribute("class"), "contents"))
            break;
    }
    
    if (! tempElement)
    {
        cout << "GetExtractFunctionsInfo : Could not get the tag div[class:contents]" << endl;
        return false;
    }
    
    
    tempElement = tempElement->FirstChildElement("ul");

    if (! tempElement)
    {
        cout << "GetExtractFunctionsInfo : Could not get the tag ul" << endl;
        return false;
    }

    

    tempElement = tempElement->FirstChildElement("li");
    const char * tempFunctionName;
    const char * tempFunctionLink;
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement("li"))
    {
        tempFunctionName = tempElement->GetText();

        tempElement2 = tempElement->FirstChildElement("a");
        tempFunctionLink = tempElement2->Attribute("href");

        cout << "Function Name : " << tempElement->GetText() << "\n Function Link : " << tempElement2->Attribute("href") << endl;

        if (! ExtractFunctionInfo(tempElement->GetText(), tempElement2->Attribute("href"), tempFunctionInfo))
            continue;

        //
    }
    
    
    tempFunctionInfo.FunctionName = NULL;


    cout << "I am still fine" << endl;
    
    return true;
}

bool ExtractFunctionInfo(const char * xFunctionName, const char * xFunctionLink, FunctionInfo_Struct & functionInfo)
{
    if (! xFunctionName || ! *xFunctionName || ! xFunctionLink || ! *xFunctionLink)
        return false;

    xFunctionLink = strstr(xFunctionLink, "#");
    
    if (! strstr(xFunctionName, "(") || ! xFunctionLink)
        return false;

    functionInfo.FunctionName = new char[strlen(xFunctionName)];
    
    char aa[1000];
    sscanf("\r\nabc()ad   a", " %[^(]", aa);
    cout << "sscanf => #" << aa << "#" << endl;
    return true;
}
