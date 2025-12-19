/*
yacc --yacc -d --debug parser.y -o y.tab.cpp
echo 'step-1: y.tab.cpp and y.tab.hpp created'
flex -o parser.cpp lexer.l
echo 'step-2: scanner created'
g++ -w *.cpp
echo 'step-3: a.out created'
rm parser.cpp y.tab.cpp y.tab.hpp
./a.out input.c
rm a.out 



*/





%{
#include<bits/stdc++.h>
#include "ParseTree.h"
#include "HelperFunctions.h"
#include "SymbolTable.h"
#include "AssemblyGenerator.h"
using namespace std;
int yyparse(void);
int yylex(void);
extern FILE *yyin;
extern int yylineno;
ofstream parsetree_file;
ofstream log_file;
ofstream assembly_file;
ofstream error_file;
int bucketSize=11;
extern expression expressionValues;
int errorCount=0;
SymbolTable *table = new SymbolTable(bucketSize);
HelperFunction *h= new HelperFunction();
int errorLine;
int labelCount=1;
string currLevel;
void yyerror(char *s)
{
    errorLine=yylineno;
    errorCount++;
    log_file<<"Error at line no "<<yylineno<<" : syntax error"<<endl;
}
string getNewLabel()
{
    string temp = "L";
    temp.append(to_string(labelCount)+":");
    labelCount++;
    //this->Label = temp;
    return temp;
    }


vector<SymbolInfo*> *func_params = new vector<SymbolInfo*>();
vector<SymbolInfo*> *current_dec_list = new vector<SymbolInfo*>();
SymbolInfo* current_function=nullptr;
vector<SymbolInfo*> *delete_symbol_list=new vector<SymbolInfo*>();
bool functionParsed=false;
//bool arrayCheck=false;
string fType="";

void erm_s(SymbolInfo* s) 
{
    if(s!=nullptr)
    { 
       // cout<<1;
        delete s;
        s=nullptr;
    }
}

void erm_h(ParseTree* p) 
{
    if(p!=nullptr)
    {
        delete p;
        p=nullptr;
    }
}

void printError(string str,int line)
{
    error_file<<"Line# "<<line<<": "<<str<<endl;    
}

void delete_symbol(vector<SymbolInfo*>* symbol)
{
    for(int i=0;i<symbol->size();i++)
    {

    }
    delete symbol;
}

void freeMemory()
{
    
        delete table;
        delete_symbol(delete_symbol_list);
        delete func_params;
        delete current_dec_list;
        delete h;

}

%}


%union{
    ParseTree* parseTree;
    SymbolInfo* symbol;
}

%token<symbol> ID LPAREN RPAREN SEMICOLON COMMA LCURL RCURL LSQUARE RSQUARE CONST_INT CONST_FLOAT 
%token<symbol> INT FLOAT VOID FOR IF ELSE LOWER_THAN_ELSE WHILE PRINTLN RETURN
%token<symbol> ADDOP MULOP RELOP LOGICOP ASSIGNOP INCOP DECOP NOT
%type <parseTree> start variable factor term unary_expression simple_expression rel_expression logic_expression expression
%type <parseTree> expression_statement statement statements compound_statement
%type <parseTree> type_specifier var_declaration func_declaration func_definition unit program 
%type <parseTree>  declaration_list parameter_list argument_list arguments M N
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start : program
	{
        ParseTree *tree = new ParseTree();
        $$ =new ParseTree(false,"start : program",$1->getLineBegin(),$1->getLineEnd());
        $$->addChild($1);
        tree->setHead($$);
        tree->printParseTree(parsetree_file);
        AssemblyGenerator *a = new AssemblyGenerator(table);
        a->GenerateAssembly($$);
        a->optimize_code();
        delete a;
        log_file<<"start : program "<<endl;
        delete tree;
        freeMemory();
        errorCount+=h->returnErrorCount();
        cout<<"Error count "<<errorCount<<endl;
        log_file<<"Total Lines: "<<yylineno<<endl;
        log_file<<"Total Errors: "<<errorCount<<endl;

    }
	;

program : program unit 
    { 
        $$ =new ParseTree(false,"program : program unit",$1->getLineBegin(),$2->getLineEnd());
        $$->addChild($1)->addChild($2);
        log_file<<"program : program unit "<<endl;
    }
	| unit
    {
        $$ =new ParseTree(false,"program : unit",$1->getLineBegin(),$1->getLineEnd());
        $$->addChild($1);
        log_file<<"program : unit "<<endl;
    }
	;
	
unit : var_declaration
    {
        $$ =new ParseTree(false,"unit : var_declaration",$1->getLineBegin(),$1->getLineEnd());
        $$->addChild($1);
        log_file<<"unit : var_declaration  "<<endl;
    }    
    | func_declaration
    {
        $$ =new ParseTree(false,"unit : func_declaration",$1->getLineBegin(),$1->getLineEnd());
        $$->addChild($1);
        log_file<<"unit : func_declaration "<<endl;
    }
    | func_definition
    {
        $$ =new ParseTree(false,"unit : func_definition",$1->getLineBegin(),$1->getLineEnd());
        $$->addChild($1);
        log_file<<"unit : func_definition  "<<endl;
    }
    ;
     