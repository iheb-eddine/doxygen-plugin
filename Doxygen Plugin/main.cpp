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
#include <algorithm>

#include "tinyxml.h"

#include "utils.h"

using namespace std;


int main(int argc, const char * argv[])
{
    std::string htmlDoxygenOutputDirPath = "/Users/ihebeddine/projects/doxygen/SamllExample/html";
    std::string funcExtraDescDirPath = "/Users/ihebeddine/projects/doxygen/SamllExample";

    std::vector<FunctionInfo_Struct> functionList;

    GetExtractFunctionsInfo(htmlDoxygenOutputDirPath, functionList);

    InjectExtraDescIntoHtmlFiles(functionList, htmlDoxygenOutputDirPath, funcExtraDescDirPath);
    
    
    return 0;
}


void ReadFileContentIntoString(std::string fileName, std::string & content)
{
    ifstream ifs(fileName);
    content.assign(istreambuf_iterator<char>(ifs) , istreambuf_iterator<char>());
}

bool GetExtractFunctionsInfo(std::string htmlDirPath, std::vector<FunctionInfo_Struct> & functionList)
{
    string globalsFuncContent;

    functionList.clear();

    ReadFileContentIntoString(htmlDirPath + "/globals_func.html", globalsFuncContent);
    
    cout << globalsFuncContent << endl;
    
    TiXmlDocument doc;
    const char * strError = doc.Parse(globalsFuncContent.c_str());
    TiXmlElement * RootElement, * tempElement, * tempElement2;
    FunctionInfo_Struct tempFunctionInfo;
    
    if (strError == NULL)
    {
        std::cout << "Could not parse the file " << globalsFuncContent << endl;
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
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement("li"))
    {
        tempElement2 = tempElement->FirstChildElement("a");

        cout << "Function Name : " << tempElement->GetText() << "\n Function Link : " << tempElement2->Attribute("href") << endl;

        if (! ExtractFunctionInfo(tempElement->GetText(), tempElement2->Attribute("href"), tempFunctionInfo))
            continue;

        functionList.push_back(FunctionInfo_Struct(tempFunctionInfo));
    }


    std::sort(functionList.begin(), functionList.end(),
              [] (const FunctionInfo_Struct & st1, const FunctionInfo_Struct & st2)
              {
                  return strcmp(st1.FileName, st2.FileName) < 0;
              }
              );


    cout << "I am still fine" << endl;
    
    return true;
}

bool ExtractFunctionInfo(const char * xFunctionName, const char * xFunctionLink, FunctionInfo_Struct & functionInfo)
{
    if (! xFunctionName || ! *xFunctionName || ! xFunctionLink || ! *xFunctionLink)
        return false;

    char * functionId = strstr((char *)xFunctionLink, "#");

    if (! strstr(xFunctionName, "(") || (unsigned int)(strstr(xFunctionName, "(") - xFunctionName) >= sizeof(functionInfo.FunctionName) || ! functionId || ! functionId[1] || functionId == xFunctionLink)
        return false;

    sscanf(xFunctionName, " %[^(]", functionInfo.FunctionName);

    sscanf(xFunctionLink, " %[^#]#%s", functionInfo.FileName, functionInfo.FunctionId);
    //strcpy(functionInfo.FunctionId, xFunctionLink + 1);

    cout << "sscanf => #" << functionInfo.FunctionName << "#" << functionInfo.FileName << "#" << functionInfo.FunctionId << "#" << endl;

    return true;
}


void InjectExtraDescIntoHtmlFiles(std::vector<FunctionInfo_Struct> & functionList, std::string htmlDirPath, std::string funcExtraDirPath)
{
    std::string currentFileName = "";
    
    TiXmlDocument xmlDoc;
    TiXmlElement * contentDivElement = NULL, * extraDescElement = NULL; //    html/body/div:[class=content]

    bool isHtmlDocEdited = false;
    
    for (auto itr = functionList.begin(); itr != functionList.end(); itr++)
    {
        if (currentFileName.compare(itr->FileName))
        {
            if (isHtmlDocEdited)
            {
                SaveHtmlDoc(xmlDoc, htmlDirPath+ "/" + currentFileName);
            }
            
            currentFileName = itr->FileName;
            isHtmlDocEdited = false;

            //If this function fails, contentDivElement will be NULL, in this case we will ignore all functins among this file
            GetContentDivElement(htmlDirPath+ "/" + itr->FileName, xmlDoc, contentDivElement);
        }

        if (! contentDivElement)
            continue;


        ifstream functionTextFile((funcExtraDirPath + "/" + itr->FunctionName + ".txt").c_str());
        cout << " ABC : " << funcExtraDirPath + "/" + itr->FunctionName + ".txt" << endl;
        if (! functionTextFile.good())
        {
            functionTextFile.close();
            continue;
        }

        FindExtraDescElementPosition(contentDivElement, itr->FunctionId, extraDescElement);

        if (! extraDescElement)
            goto next_loop;

        //std::getLine(functionTextFile, )
        if (! InsertFuncExtraDescIntoHtmlDoc(functionTextFile, extraDescElement))
            goto next_loop;

        isHtmlDocEdited = true;
        
    next_loop:
        functionTextFile.close();
    
    }
    
    if (isHtmlDocEdited)
    {
        SaveHtmlDoc(xmlDoc, htmlDirPath+ "/" + currentFileName);
    }
}

bool InsertFuncExtraDescIntoHtmlDoc(std::ifstream & functionTextFile, TiXmlElement * extraDescElement)
{
    //extraDescElement->LinkEndChild(new TiXmlText("Bonjour tout le monde" ));
    return true;
}


void SaveHtmlDoc(TiXmlDocument & xmlDoc, std::string htmlFilePath)
{
    xmlDoc.SaveFile(htmlFilePath.c_str());
}


void FindExtraDescElementPosition(TiXmlElement * contentDivElement, const char * functionId, TiXmlElement *& extraDescElement)
{
    extraDescElement = NULL;
    
    
    TiXmlElement * tempElement, *tempElement2, * tempSectinoElement;
    TiXmlElement extraDescSectionElement("dl");

    tempElement = contentDivElement->FirstChildElement("a");
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement())
    {
        if (tempElement->Attribute("id") && ! strcmp(tempElement->Attribute("id"), functionId))
            break;
    }
    
    if (! tempElement)
    {
        cout << "FindExtraDescElementPosition : Could not get the tag a[id:" << functionId << "]" << endl;
        return;
    }
    
    
    tempElement = tempElement->NextSiblingElement();
    
    if (! tempElement || strcmp(tempElement->Value(), "div") || ! tempElement->Attribute("class") || strcmp(tempElement->Attribute("class"), "memitem"))
        return;

    //Searching div:[class=memdoc]

    tempElement = tempElement->FirstChildElement("div");
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement())
    {
        if (tempElement->Attribute("class") && ! strcmp(tempElement->Attribute("class"), "memdoc"))
            break;
    }
    
    if (! tempElement)
    {
        cout << "FindExtraDescElementPosition : Could not get the tag div[class:memdoc]" << endl;
        return;
    }


    extraDescSectionElement.SetAttribute("class", "section extra");

    tempElement2 = (TiXmlElement *)extraDescSectionElement.InsertEndChild(TiXmlElement("dt"));
    if (! tempElement2)
        return;

    tempElement2->LinkEndChild(new TiXmlText("Extra Description"));

    ////Check this
    //extraDescElement = (TiXmlElement *)extraDescSectionElement.LinkEndChild(new TiXmlElement("dd"));



    tempSectinoElement = tempElement->FirstChildElement("dl");

    for (; tempSectinoElement; tempSectinoElement = tempSectinoElement->NextSiblingElement("dl"))
    {
        if (tempSectinoElement->Attribute("class") && ! strcmp(tempSectinoElement->Attribute("class"), "section return"))
            break;
    }
    
    if (tempSectinoElement)
    {
        goto insert_after;
    }
    
    
    tempSectinoElement = tempElement->FirstChildElement("dl");
    
    for (; tempSectinoElement; tempSectinoElement = tempSectinoElement->NextSiblingElement("dl"))
    {
        if (tempSectinoElement->Attribute("class") && ! strcmp(tempSectinoElement->Attribute("class"), "section params"))
            break;
    }
    
    if (tempSectinoElement)
    {
        goto insert_after;
    }
    
    
    tempSectinoElement = tempElement->FirstChildElement("dl");
    
    for (; tempSectinoElement; tempSectinoElement = tempSectinoElement->NextSiblingElement("dl"))
    {
        if (tempSectinoElement->Attribute("class") && ! strcmp(tempSectinoElement->Attribute("class"), "section see"))
            break;
    }
    
    if (tempSectinoElement)
    {
        goto insert_before;
    }
    
    
    tempSectinoElement = tempElement->FirstChildElement("dl");
    
    for (; tempSectinoElement; tempSectinoElement = tempSectinoElement->NextSiblingElement("dl"))
    {
        if (tempSectinoElement->Attribute("class") && ! strcmp(tempSectinoElement->Attribute("class"), "section note"))
            break;
    }
    
    if (tempSectinoElement)
    {
        goto insert_before;
    }
    
    
    tempSectinoElement = tempElement->FirstChildElement("dl");
    
    for (; tempSectinoElement; tempSectinoElement = tempSectinoElement->NextSiblingElement("dl"))
    {
        if (tempSectinoElement->Attribute("class") && ! strcmp(tempSectinoElement->Attribute("class"), "section warning"))
            break;
    }
    
    if (tempSectinoElement)
    {
        goto insert_before;
    }

    goto insert_at_end;

insert_after:

    tempElement = (TiXmlElement *)tempElement->InsertAfterChild(tempSectinoElement, extraDescSectionElement);
    goto after_insertion;


insert_before:

    tempElement = (TiXmlElement *)tempElement->InsertBeforeChild(tempSectinoElement, extraDescSectionElement);
    goto after_insertion;


insert_at_end:
    tempElement = (TiXmlElement *)tempElement->InsertEndChild(extraDescSectionElement);

after_insertion:

    
    //// Check this
    
    if (! tempElement)
    {
        extraDescElement = NULL;
        return;
    }
    
    extraDescElement = (TiXmlElement *)tempElement->LinkEndChild(new TiXmlElement("dd"));
}


void GetContentDivElement(std::string htmlFilePath, TiXmlDocument & xmlDoc, TiXmlElement *& contentDivElement)
{
    contentDivElement = NULL;

    string htmlFileContent;
    htmlFileContent.clear();
    
    ReadFileContentIntoString(htmlFilePath, htmlFileContent);
    
    const char * strError = xmlDoc.Parse(htmlFileContent.c_str());
    TiXmlElement * tempElement;

    //cout << "htmlFilePath : " << htmlFilePath;
    
    if (strError == NULL)
    {
        std::cout << "Could not parse the file " << htmlFilePath << endl;
        return;
    }

    tempElement = xmlDoc.RootElement();



    if (strcmp(tempElement->Value(), "html"))
    {
        cout << "GetContentDivElement : Could not get the tag html" << endl;
        return;
    }
    
    
    tempElement = tempElement->FirstChildElement("body");
    
    if (! tempElement)
    {
        cout << "GetContentDivElement : Could not get the tag body" << endl;
        return;
    }
    
    
    tempElement = tempElement->FirstChildElement("div");
    
    for (; tempElement; tempElement = tempElement->NextSiblingElement())
    {
        if (tempElement->Attribute("class") && ! strcmp(tempElement->Attribute("class"), "contents"))
            break;
    }
    
    if (! tempElement)
    {
        cout << "GetContentDivElement : Could not get the tag div[class:contents]" << endl;
        return;
    }

    contentDivElement = tempElement;
}

