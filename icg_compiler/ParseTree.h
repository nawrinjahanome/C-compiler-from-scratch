#pragma once
#include <bits/stdc++.h>
using namespace std;
typedef struct expression
{
    string id;
    float value;
    string dataType;
    bool isZero;
}expression;

class ParseTree
{
private:
    ParseTree *head;
    vector<ParseTree *> *children;
    bool isTerminal;
    string rule;
    int lineBegin;
    int lineEnd;
    string dataType;
    expression exp;
    string var_type;
    vector<expression> *expressionList;
    string nextLevel;
    bool pushed;
    string code;
    bool relationalExpression;
    string arrayCode;
    vector<string> *truelist, *falselist, *nextlist;


public:
    int offset;
    string arrayType;
    string sd;
    bool relationalExpressionEvaluated = false;
    ParseTree(bool isTerminal, string rule, int lineBegin, int lineEnd)
    {
        arrayCode="";
        truelist = new vector<string>();
        falselist = new vector<string>();
        nextlist = new vector<string>();
        this->pushed=false;
        this->isTerminal = isTerminal;
        this->rule = rule;
        this->lineBegin = lineBegin;
        this->lineEnd = lineEnd;
        this->children = new vector<ParseTree *>();
        dataType="";
        expressionList = new vector<expression>();
        relationalExpression=false;
    }
    string getArrayCode()
    {
        return this->arrayCode;
    }
    void setArrayCode(string arrayCode)
    {
        this->arrayCode = arrayCode;
    }

    bool getRelationalExpression()
    {
        return this->relationalExpression;
    }
    void setRelationalExpression(bool relationalExpression)
    {
        this->relationalExpression = relationalExpression;
    }

    void setPushed(bool pushed)
    {
        this->pushed = pushed;
    }
    bool getPushed()
    {
        return this->pushed;
    }
    string getCode()
    {
        return this->code;
    }
    void setCode(string code)
    {
        this->code = code;
    }
    
    void add_to_truelist(string line) { truelist->push_back(line); }
    void add_to_falselist(string line) { falselist->push_back(line); }
    void add_to_nextlist(string line) { nextlist->push_back(line); }
    void set_truelist(vector<string> *list) 
    { 
        this->truelist->clear();
        this->truelist->reserve(list->size());
        for(int i=0;i<list->size();i++)
        {
            add_to_truelist(list->at(i));
        }

    }
    void set_falselist(vector<string> *list) 
    { 
        this->falselist->clear();
        this->falselist->reserve(list->size());
        for(int i=0;i<list->size();i++)
        {
            add_to_falselist(list->at(i));
        }
    }
    void set_nextlist(vector<string> *list) 
    { 
        this->nextlist->clear();
        this->nextlist->reserve(list->size());
        for(int i=0;i< list->size();i++)
        {
            add_to_nextlist(list->at(i));
        }
    }

    vector<string>* get_truelist() { return this->truelist; }
    vector<string>* get_falselist() { return this->falselist; }
    vector<string>* get_nextlist() { return this->nextlist; }


    string getNextLevel()
    {
        return this->nextLevel;
    }
    void setNextLevel(string nextLevel)
    {
        this->nextLevel = nextLevel;
    }
    
    string getVarType()
    {
        return this->var_type;
    }

    void setVarType(string var_type)
    {
        this->var_type = var_type;
    }
    
    ParseTree()
    {
        this->children = new vector<ParseTree *>();
    }

    ~ParseTree()
    {
        while (!children->empty())
        {
            delete children->back();
            children->pop_back();
        }
        delete children;
        delete expressionList;
    }

    void addExpression(expression exp)
    {
        expressionList->push_back(exp);
    }

    void setExp(expression exp)
    {
        this->exp = exp;
    }

    void setExp2(string id, string dataType)
    {
        this->exp.id = id;
        this->exp.dataType = dataType;
    }

    expression getExp()
    {
        return this->exp;
    }

    vector<expression> *getExpressionList()
    {
        return expressionList;
    }

    void removeExpressionList()
    {
        while(!expressionList->empty())
        {
        expressionList->pop_back();
        }
    }

    void setHead(ParseTree *head)
    {
        this->head = head;
    }

    ParseTree *getHead()
    {
        return this->head;
    }

    string getRule()
    {
        return this->rule;
    }

    ParseTree *addChild(ParseTree *child)
    {
        this->children->push_back(child);
        return this;
    }

    vector<ParseTree *> *getChildren()
    {
        return this->children;
    }

    bool getIsTerminal()
    {
        return this->isTerminal;
    }

    int getLineBegin()
    {
        return this->lineBegin;
    }

    void setDataType(string dataType)
    {
        this->dataType = dataType;
    }

    string getDataType()
    {
        return this->dataType;
    }


    int getLineEnd()
    {
        return this->lineEnd;
    }

    static void printParseTreeRecursive(ofstream &outputFile, ParseTree *node, int depth)
    {

        if (node->getIsTerminal())
        {
            for (int i = 0; i < depth; i++)
            {
                outputFile << " ";
            }
            outputFile << node->getRule() << "\t<Line: " << node->getLineBegin() << ">" << endl;
            return;
        }
        for (int i = 0; i < depth; i++)
        {
            outputFile << " ";
        }
        outputFile << node->getRule() << " \t<Line: " << node->getLineBegin() << "-" << node->getLineEnd() << ">" << endl;
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            printParseTreeRecursive(outputFile, node->getChildren()->at(i), depth + 1);
        }
    }

     void printParseTree(ofstream &outputFile)
    {
        printParseTreeRecursive(outputFile, this->getHead(), 0);
    }
};
