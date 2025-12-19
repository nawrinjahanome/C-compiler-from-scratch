#pragma once
#include <bits/stdc++.h>
#include "ParseTree.h"
#include "SymbolInfo.h"
#include "SymbolTable.h"
using namespace std;
static int errorcount = 0;
class HelperFunction
{
private:
    string redif_error_param = "";


public:
    int returnErrorCount()
    {
        return errorcount;
    }
    bool checkRedifinition(vector<SymbolInfo *> *params)
    {
        for (int i = 0; i < params->size(); i++)
        {
            for (int j = i + 1; j < params->size(); j++)
            {
                if (params->at(i)->getName() == params->at(j)->getName())
                {
                    redif_error_param = params->at(i)->getName();
                    for (int k = j + 1; k < params->size(); k++)
                    {
                      //  cout << "deleting " << params->at(k)->getName() << endl;
                        params->erase(params->begin() + j + 1, params->end());
                    }
                    return true;
                }
            }
        }
    }

    string checkType(string type1, string type2)
    {
        if(type1=="NOT DECLARED" || type2=="NOT DECLARED" || type1=="NOT ARRAY" || type2=="NOT ARRAY" || type1=="NOT FUNCTION" || type2=="NOT FUNCTION")
        {
            return "ERROR";
        }

        if(type1=="VOID"|| type2=="VOID")
        {
            return "VOID";
        }
        if (type1 == "FLOAT" || type2 == "FLOAT")
        {
            return "FLOAT";
        }
        else
        {
            return "INT";
        }
    }

    bool checkSameSymbol(SymbolTable *symbolTable, string functionName)
    {
        SymbolInfo *symbol = symbolTable->lookup(functionName);
        if (symbol != nullptr && symbol->getSymbolType() != "FUNCTION")
        {
            redif_error_param = functionName;
            return true;
        }
        return false;
    }



    bool checkConflictingType(SymbolTable *symbolTable, string dataType, string Name, string type)
    {
        //cout<<"check conflicting typeee"<<endl;
        SymbolInfo *symbol = symbolTable->lookup(Name);
        if (symbol != nullptr && symbol->getDataType() != dataType && symbol->getSymbolType() == type)
        {
            redif_error_param = Name;
           // cout<<Name<<endl;
          //  cout<<symbol->getDataType()<<" "<<endl;
          //  cout<<symbol->getSymbolType()<<" "<<endl;

            return true;
        }
        return false;
    }

    bool checkConflictingTypeFunctionParameter(SymbolTable* SymbolTable,vector<SymbolInfo*> *params,string name)
    {
        SymbolInfo *symbol = SymbolTable->lookup(name);
        if(symbol==nullptr)
        {
            return false;
        }
        vector<SymbolInfo*> *parameterList=symbol->getParameterList();
        if(params->size()!=parameterList->size())
        {
            redif_error_param=name;
            return true;
        }

        for(int i=0;i<params->size();i++)
        {
            if(params->at(i)->getDataType()!=parameterList->at(i)->getDataType())
            {
                redif_error_param=name;
                return true;
            }
        }
        return false;


    }

    bool checkSameDataType(SymbolTable *symbolTable, string dataType, string Name)
    {
        SymbolInfo *symbol = symbolTable->lookupCurrent(Name);
        if (symbol != nullptr && symbol->getDataType() != dataType)
        {
            // redif_error_param = Name;
            return true;
        }
        return false;
    }

    void checkFunction(SymbolInfo* function,vector<expression>* arguments,ofstream &errorfile,int lineNo)
    {
        int size1=function->getParameterList()->size();
        int size2=arguments->size();
        if(size1>size2)
        {
            printError("Too few arguments to function \'"+function->getName()+"\'",lineNo,errorfile);
            
        }  
        else if(size1<size2)
        {
            printError("Too many arguments to function \'"+function->getName()+"\'",lineNo,errorfile);

        }
        else
        {
            for(int i=0;i<size1;i++)
            {
                if(function->getParameterList()->at(i)->getDataType()!=arguments->at(i).dataType)
                {
                    printError("Type mismatch for argument "+to_string(i+1)+" of \'"+function->getName()+"\'",lineNo,errorfile);
                }
            }
        }
        return ;
    }



    SymbolInfo *insertFunctionSymbolTable(SymbolInfo *sym, vector<SymbolInfo *> *params, string functionType, string dataType)
    {
        if (true)
        {
            SymbolInfo *symbol = new SymbolInfo(sym->getName(), sym->getType());
            symbol->setLineNo(sym->getLineNo());
            symbol->setFunctionStatus(functionType);
            symbol->setParameterList(params);
            symbol->setDataType(dataType);
            symbol->setSymbolType("FUNCTION");
            return symbol;
        }
    }

    void printSameSymbolError(string errorType, int lineNo, ofstream &errorfile)
    {
        errorcount++;
        errorfile << "Line# " << lineNo << ": "
                  << "\'" << redif_error_param << "\' " << errorType << endl;
        redif_error_param = "";
    }

    void printErrorRedif(string errorType, int lineNo, ofstream &errorfile)
    {
        errorcount++;
        errorfile << "Line# " << lineNo << ": " << errorType << " "
                  << "\'" << redif_error_param << "\'" << endl;
        redif_error_param = "";
    }

    void printError(string errorType, int lineNo, ofstream &errorfile)
    {
        errorfile << "Line# " << lineNo << ": " << errorType << endl;
        errorcount++;
    }

    string expressionResult(string type1, string type2)
    {
        if (type1 == "FLOAT" || type2 == "FLOAT")
        {
            return "FLOAT";
        }
        else
        {
            return "INT";
        }
    }

};
