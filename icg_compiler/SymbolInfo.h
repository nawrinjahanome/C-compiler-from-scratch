#pragma once
#include <bits/stdc++.h>
using namespace std;

class SymbolInfo
{
private:
    string name;
    string type;
    SymbolInfo *next;
    int lineNo;
    string dataType;
    string symbol;
    string status;
    int offset;
    int arrayOffset;
    vector<SymbolInfo *> *parameterList;

public:

    void setOffset(int offset)
    {
        this->offset = offset;
    }

    int getOffset()
    {
        return this->offset;
    }

    void setArrayOffset(int arrayOffset)
    {
        this->arrayOffset = arrayOffset;
    }

    int getArrayOffset()
    {
        return this->arrayOffset;
    }

    SymbolInfo()
    {
        name = "";
        type = "";
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        symbol = "VARIABLE";
    }

    SymbolInfo(string name, string type)
    {
        this->name = name;
        this->type = type;
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        symbol = "VARIABLE";
        // dataType = type;
    }
    SymbolInfo(string name, string type, string dataType)
    {
        this->name = name;
        this->type = type;
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        symbol = "VARIABLE";
        this->dataType = dataType;
    }
    SymbolInfo(string name, string type, string dataType, string symbol)
    {
        this->name = name;
        this->type = type;
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        this->dataType = dataType;
        this->symbol = symbol;
    }
    SymbolInfo(string name, string type, string dataType, string symbol, string status)
    {
        this->name = name;
        this->type = type;
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        this->dataType = dataType;
        this->symbol = symbol;
        this->status = status;
    }
    SymbolInfo(string name, string type, string dataType, string symbol, string status, int lineNo)
    {
        this->name = name;
        this->type = type;
        next = nullptr;
        parameterList = new vector<SymbolInfo *>();
        this->dataType = dataType;
        this->symbol = symbol;
        this->status = status;
        this->lineNo = lineNo;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setType(string type)
    {
        this->type = type;
    }

    void setNext(SymbolInfo *next)
    {
        this->next = next;
    }
    void setLineNo(int lineNo)
    {
        this->lineNo = lineNo;
    }
    int getLineNo()
    {
        return this->lineNo;
    }

    string getName()
    {
        return name;
    }

    string getType()
    {
        return type;
    }

    SymbolInfo *getNext()
    {
        return next;
    }

    void setDataType(string dataType)
    {
        this->dataType = dataType;
    }

    string getDataType()
    {
        return this->dataType;
    }

    void setSymbolType(string symbol)
    {
        this->symbol = symbol;
    }

    string getSymbolType()
    {
        return this->symbol;
    }

    void setFunctionStatus(string status)
    {
        this->status = status;
    }

    string getFunctionStatus()
    {
        return this->status;
    }

    void setParameterList(vector<SymbolInfo *> *parameterList)
    {
        delete this->parameterList;
        this->parameterList = parameterList;
    }

    void addParameter(SymbolInfo *data)
    {
        parameterList->push_back(data);
    }

    vector<SymbolInfo *> *getParameterList()
    {
        if (parameterList == nullptr)
        {
            return new vector<SymbolInfo *>();
        }
        return parameterList;
    }

    ~SymbolInfo()
    {
        if (next != nullptr)
        {
           // delete next;
        }
        for(int i=0;i<parameterList->size();i++)
        {
            delete parameterList->at(i);
        }
    }
};
