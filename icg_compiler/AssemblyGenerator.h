#pragma once
#include <bits/stdc++.h>
#include "ParseTree.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"
using namespace std;
class AssemblyGenerator
{
    SymbolTable *table;
    int currentOffset;
    ofstream file;
    ofstream tempfile;
    vector<string> strings;
    string functionName;
    int totalOffset;
    SymbolTable *tempTable;
    string currentLocalVar;
    string AX;
    string BX;
    string CX;
    string DX;
    bool incFlag = false;
    vector<string> expressionStack;
    int lineNo = 1;
    string Label;
    string dummyLabel;
    int labelCount = 1;
    int dummyLabelCount = 1;
    bool simpleExpression;
    bool relationalExpression = true;
    vector<pair<string, string>> *backpatch;
    vector<pair<string, int>> *functionParams;
    string returnLabel;
    vector<string> *functionEnd;
    int returnOffset;

public:
    AssemblyGenerator(SymbolTable *table)
    {

        // merge=new vector<string>;
        backpatch = new vector<pair<string, string>>();
        functionEnd = new vector<string>();
        this->table = table;
        currentOffset = 0;
        totalOffset = 0;
        file.open("code_unoptimized.asm");
        tempfile.open("TempAssembly.txt");
        AX = "";
        BX = "";
        CX = "";
        DX = "";
        simpleExpression = true;
        relationalExpression = false;
        functionParams = new vector<pair<string, int>>;
    }
    string getDummyLabel()
    {
        return this->dummyLabel;
    }
    string getNewDummyLabel()
    {
        string temp = "TL";
        temp.append(to_string(dummyLabelCount));
        dummyLabelCount++;
        this->dummyLabel = temp;
        return temp;
    }

    string getLabel()
    {
        return this->Label;
    }
    string getNewLabel()
    {
        string temp = "L";
        temp.append(to_string(labelCount) + ":");
        labelCount++;
        this->Label = temp;
        return temp;
    }

vector<string> trim_and_get_operands(const string& line) {
    vector<char> chars_to_trim = {' ', '\t', '\r', '\n'};
    vector<string> operands;
    int start = 0;
    int line_size = line.size();
    while (start < line_size && find(chars_to_trim.begin(), chars_to_trim.end(), line[start]) != chars_to_trim.end()) {
        start++;
    }
    int idx = start;
    while (idx < line_size) {
        int operand_start = idx;
        while (idx < line_size && line[idx] != ' ') {
            idx++;
        }
        if (idx > operand_start) {
            string operand = line.substr(operand_start, idx - operand_start);
            if (operand.back() == ',') {
                operand.pop_back();
            }
            operands.push_back(operand);
        }
        idx++;
    }

    return operands;
}

void optimize_code() {
    ifstream fin;
    ofstream fout;
    fin.open("code_unoptimized.asm");
    fout.open("code_optimized.asm");
    string line;
    while (getline(fin, line)) {
        vector<string> operands = trim_and_get_operands(line);

        if (!operands.empty()) {
         if (operands.size() == 3 && operands.front() == "ADD" && operands[2] == "0")
                continue;
            else if (operands.size() == 3 && operands.front() == "MUL" && operands[2] == "1")
                continue;
            else if (operands.front() == "PUSH") {
                vector<string> prev_operands = operands;
                string prev_line = line;
                if (getline(fin, line)) {
                    operands = trim_and_get_operands(line);
                    if (operands.front() == "POP" && operands[1] == prev_operands[1]) continue;
                    else {
                        fout << prev_line << endl;
                        fout << line << endl;
                        continue;
                    }
                } else {
                    fout << prev_line << endl;
                    continue;
                }
            }
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
}

    void customSplit(string str, char separator)
    {
        int startIndex = 0, endIndex = 0;
        for (int i = 0; i <= str.size(); i++)
        {
            if (str[i] == separator || i == str.size())
            {
                endIndex = i;
                string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                strings.push_back(temp);
                startIndex = endIndex + 1;
            }
        }
    }

    void printVar(string symbol)
    {
        SymbolInfo *info = table->lookup(symbol);
        if (info != nullptr)
        {
            if (info->getSymbolType().find("ARRAY") != string::npos)
            {
                currentOffset += info->getArrayOffset() * 2;
                info->setOffset(currentOffset);
                file << "\t" << symbol << " DW " << info->getArrayOffset() << " DUP (0000H)" << endl;
            }
            else
            {
                currentOffset += 2;
                info->setOffset(currentOffset);
                file << "\t" << symbol << " DW 1 DUP (0000H)" << endl;
            }
        }
    }

    void getFunctionParams(ParseTree *node)
    {
        if (node->getIsTerminal())
        {
            if (node->getRule().find("ID :") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                // printVar(str);
                strings.clear();
                returnOffset += 2;
                functionParams->push_back(make_pair(str, returnOffset));
            }
            return;
        }
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            getFunctionParams(node->getChildren()->at(i));
        }
    }

    void printGlobalVariable(ParseTree *node)
    {
        if (node->getIsTerminal())
        {
            if (node->getRule().find("ID :") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                printVar(str);
                strings.clear();
            }
            return;
        }
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            printGlobalVariable(node->getChildren()->at(i));
        }
    }
    void printReturn(string functionName)
    {
        tempfile << getNewLabel() << endl;
        tempfile << generateAddCode("SP", to_string(totalOffset));
        tempfile << "\tPOP BP" << endl;
        if (functionName == "main")
        {
            tempfile << "\tMOV AX,4CH" << endl;
            tempfile << "\tINT 21H" << endl;
        }
        else
        {
            string s = returnOffset == 0 ? " " : to_string(returnOffset);
            tempfile << "\tRET " << s << endl;
        }
        tempfile << functionName << " ENDP" << endl;
    }

    void printFunctionHeader(string str)
    {

        tempfile << str << " PROC" << endl;
        if (str == "main")
        {
            tempfile << "\tMOV AX, @DATA" << endl;
            tempfile << "\tMOV DS, AX" << endl;
        }
        tempfile << "\tPUSH BP" << endl;
        tempfile << "\tMOV BP, SP" << endl;
        lineNo += 5;
    }

    void generateAssemblyRecursive(ParseTree *node)
    {
        if (node->getIsTerminal())
        {

            return;
        }
        if (node->getRule().find("unit : var_declaration") != string::npos)
        {
            printGlobalVariable(node);
            return;
        }
        if (node->getRule().find("func_definition : type_specifier ID LPAREN RPAREN compound_statement") != string::npos)
        {
            ParseTree *temp = node->getChildren()->at(1);
            strings.clear();
            customSplit(temp->getRule(), ' ');
            string str = strings.at(2);
            functionName = str;
            printFunctionHeader(str);
            strings.clear();
            tempTable = new SymbolTable(10);
            // delete tempTable;
        }

        if (node->getRule().find("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement") != string::npos)
        {
            ParseTree *temp = node->getChildren()->at(1);
            strings.clear();
            customSplit(temp->getRule(), ' ');
            string str = strings.at(2);
            functionName = str;
            printFunctionHeader(str);
            getFunctionParams(node->getChildren()->at(3));
            // cout<<"functionParams: "<<functionParams->size()<<endl;
            strings.clear();
            tempTable = new SymbolTable(10);
        }
        if (node->getRule().find("compound_statement : LCURL statements M RCURL") != string::npos)
        {

            printFunction(node);
            delete tempTable;
            printReturn(functionName);
            backPatch(functionEnd, getLabel());
            functionParams->clear();
            functionEnd->clear();
            replaceLabels();
            totalOffset = 0;
            returnOffset = 0;
            return;
        }
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            generateAssemblyRecursive(node->getChildren()->at(i));
        }
    }

    void GenerateAssembly(ParseTree *Node)
    {
        initialize();
        generateAssemblyRecursive(Node);
        file << ".CODE" << endl;
        cout << lineNo << endl;
        writeTofile();
        printBackpatch();
    }
    void writeTofile()
    {
        ifstream tempfile2("TempAssembly2.txt");
        string line;
        while (getline(tempfile2, line))
        {
            file << line << endl;
        }
        remove("TempAssembly2.txt");
        remove("TempAssembly.txt");

        //!!!!!!!!!eikhane lekha lagbe
        file << "\n\n\n"
             << endl;
        ifstream file2("procedures.txt");
        while (getline(file2, line))
        {
            file << line << endl;
        }
        file<<"END main"<<endl;
        file2.close();
        file.close();
    }

    void replaceLabels()
    {
        string line;
        tempfile.close();
        ofstream tempfile2("TempAssembly2.txt", ios::app);
        ifstream tempfile3("TempAssembly.txt");
        while (getline(tempfile3, line))
        {
            string str1;
            string str2;
            for (int i = 0; i < backpatch->size(); i++)
            {
                str1 = backpatch->at(i).first;
                str2 = backpatch->at(i).second;
                strings.clear();
                customSplit(str1, ':');
                str1 = strings.at(0);
                strings.clear();
                customSplit(str2, ':');
                str2 = strings.at(0);
                strings.clear();
                if (line.find(str1) != string::npos)
                {
                    line.replace(line.find(str1), str1.length(), str2);
                    backpatch->erase(backpatch->begin() + i);
                    break;
                }
            }
            // cout<<"str1: "<<str1<<endl;
            // cout<<"str2: "<<str2<<endl;
            // cout<<line<<endl;
            tempfile2 << line << endl;
        }
        tempfile2.close();
        tempfile3.clear();
        tempfile.open("TempAssembly.txt");
    }

    ~AssemblyGenerator()
    {

        // file.close();
        // tempfile.close();
    }

    void initialize()
    {
        file << ".MODEL SMALL" << endl;
        file << ".STACK 1000H" << endl;
        file << ".Data" << endl;
        file << "\tnumber DB \"00000$\"" << endl;
    }

    //////////////////////!!!!!!!!!
    //////////////////////!!!!!!!!!
    //////////////////////!!!!!!!!!
    vector<string> *mergeList(vector<string> *v1, vector<string> *v2)
    {
        vector<string> *merge = new vector<string>;
        merge->clear();
        merge->reserve(v1->size() + v2->size());
        merge->insert(merge->end(), v1->begin(), v1->end());
        merge->insert(merge->end(), v2->begin(), v2->end());
        return merge;
    }

    void printList(vector<string> *list, string str)
    {
        cout << str << " :";
        for (int i = 0; i < list->size(); i++)
        {
            cout << list->at(i) << " ";
        }
        cout << endl;
    }

    void printBackpatch()
    {
        // cout << "backpatch" << endl;
        // cout << backpatch.size() << endl;
        for (int i = 0; i < backpatch->size(); i++)
        {
            string str = backpatch->at(i).first;
            string str2 = backpatch->at(i).second;
            cout << "backpatch: " << str << ": " << str2 << endl;
        }
    }
    void backPatch(vector<string> *list, string label)
    {
        for (int i = 0; i < list->size(); i++)
        {
            string str = list->at(i);
            backpatch->push_back(make_pair(str, label));
        }
    }

    void printFunction(ParseTree *node)
    {
        if (node->getIsTerminal())
        {

            if (node->getRule().find("backpatch") != string::npos)
            {
                tempfile << getNewLabel() << endl;
                lineNo++;
                node->setNextLevel(getLabel());
            }

            if (node->getRule().find("jump rule") != string::npos)
            {
                string lebel = getNewDummyLabel();
                tempfile << generateJump(lebel);
                lineNo++;
                node->add_to_nextlist(lebel);
            }

            if (node->getRule().find("CONST_INT : ") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                strings.clear();
                str = generateMoveCode("AX", str, to_string(node->getLineBegin()));
                lineNo++;
                node->setCode(str);
            }

            if (node->getRule().find("ID :") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                // cout<<"inside id: "<<node->getRule()<<endl;
                string str = strings.at(2);
                string c = str;
                // cout<<"str:"<<str<<endl;
                strings.clear();
                SymbolInfo *info = tempTable->lookup(str);
                SymbolInfo *info2 = table->lookup(str);
                if (info != nullptr)
                {
                    //  cout<<"str2:"<<str<<endl;
                    str = "[BP-" + to_string(info->getOffset()) + "]";
                    node->arrayType = "local";
                    node->offset = info->getOffset();
                }
                else
                {
                    node->arrayType = 'global';
                }
                string str2 = generateMoveCode("AX", str, to_string(node->getLineBegin()));
                if (info2 != nullptr)
                {
                    node->offset = info2->getOffset();
                }
                if (functionParams->size() != 0)
                {
                    for (int i = 0; i < functionParams->size(); i++)
                    {
                        if (functionParams->at(i).first == c)
                        {
                            node->arrayType = "";
                            int offset = 4 + returnOffset - functionParams->at(i).second;
                            str = "[BP+" + to_string(offset) + "]";
                            string str2 = generateMoveCode("AX", str, to_string(node->getLineBegin()));
                            node->setExp2(str, "ID");
                            lineNo++;
                            node->setCode(str2);
                            return;
                            break;
                        }
                    }
                }
                node->setExp2(str, "ID");
                lineNo++;
                node->setCode(str2);
            }
            if (node->getRule().find("ADDOP : ") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                node->setCode(str);
                strings.clear();
            }
            if (node->getRule().find("MULOP : ") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                node->setCode(str);
                strings.clear();
            }
            if (node->getRule().find("RELOP : ") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                node->setCode(str);
                strings.clear();
            }
            if (node->getRule().find("LOGICOP : ") != string::npos)
            {
                strings.clear();
                customSplit(node->getRule(), ' ');
                string str = strings.at(2);
                node->setCode(str);
                strings.clear();
            }
            return;
        }
        if (node->getRule().find("statement : var_declaration") != string::npos)
        {
            printLocalVar(node);
            return;
        }
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            printFunction(node->getChildren()->at(i));

            if (node->getRule().find("statement : RETURN expression SEMICOLON N") != string::npos)
            {
                if (i == 2)
                {
                    tempfile << node->getChildren()->at(1)->getCode();
                    if (expressionStack.size() != 0)
                    {
                        expressionStack.pop_back();
                        tempfile << generatePopCode("AX");
                    }
                }
            }

            if (node->getRule().find("expression : variable ASSIGNOP logic_expression") != string::npos)
            {

                if (i == 1)
                {
                    // cout << "hereeeeeeee" << endl;
                    if (node->getChildren()->at(0)->getArrayCode() != "")
                    {
                        // cout << "herexxxxxxxxxxx" << endl;
                        tempfile << node->getChildren()->at(0)->getCode();
                        tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                        // node->getChildren()->at(0)->setCode(node->getChildren()->at(0)->getArrayCode());
                    }
                }
            }

            // if (node->getRule().find("statement : IF LPAREN expression RPAREN M statement ELSE N M statement") != string::npos)
            // {
            //     if(i== 2)
            //     {
            //         cout<<"For debug"<<endl;
            //     }
            // }

            if (node->getRule().find("logic_expression : rel_expression LOGICOP M rel_expression") != string::npos)
            {
                if (i == 1)
                {
                    if (node->getChildren()->at(0)->getRelationalExpression() == false)
                    {
                        cout << "here comes " << endl;
                        // we need to generate logicop code
                        string str = node->getChildren()->at(0)->getCode();
                        if (str != "")
                        {
                            tempfile << str;
                            lineNo++;
                        }
                        else
                        {
                            tempfile << generatePopCode("AX");
                            expressionStack.pop_back();
                            lineNo++;
                        }
                        tempfile << generateCMP("AX", "0");
                        string label1 = getNewDummyLabel();
                        string label2 = getNewDummyLabel();
                        tempfile << generateJMP("!=", label1, label2);
                        lineNo += 3;
                        node->getChildren()->at(0)->setRelationalExpression(true);
                        node->getChildren()->at(0)->add_to_truelist(label1);
                        node->getChildren()->at(0)->add_to_falselist(label2);
                        node->getChildren()->at(0)->setCode("");
                    }
                }
            }
            if (node->getRule().find("factor : ID LPAREN argument_list RPAREN") != string::npos)
            {
                if(i==1)
                {
                    string str = node->getChildren()->at(0)->getRule();
                    strings.clear();
                    customSplit(str, ' ');
                    string str2 = strings.at(2);
                    cout<<"functionName: "<<str2<<endl;
                    strings.clear();
                    node->getChildren()->at(0)->setExp2(str2,"ID");
                    
                }
            }
            if (node->getRule().find("statement : FOR LPAREN expression_statement M expression_statement M expression N RPAREN M statement N") != string::npos)
            {
                if (i == 6)
                {
                    if (expressionStack.size() != 0)
                    {
                        expressionStack.pop_back();
                        tempfile << generatePopCode("AX");
                    }
                }
                if (i == 2)
                {
                    if (expressionStack.size() != 0)
                    {
                        expressionStack.pop_back();
                        tempfile << generatePopCode("AX");
                    }
                }
                if (i == 4)
                {
                    if (expressionStack.size() != 0)
                    {
                        expressionStack.pop_back();
                        tempfile << generatePopCode("AX");
                    }
                }
            }
            if (node->getRule().find("statement : WHILE M LPAREN expression RPAREN M statement") != string::npos)
            {

                if (i == 3)
                {
                    if (expressionStack.size() != 0)
                    {
                        tempfile << generatePopCode("AX");
                        expressionStack.pop_back();
                        lineNo++;
                    }
                    if (node->getChildren()->at(3)->getRelationalExpression() == false)
                    {

                        tempfile << generateCMP("AX", "0");
                        string label1 = getNewDummyLabel();
                        string label2 = getNewDummyLabel();
                        tempfile << generateJMP("!=", label1, label2);
                        lineNo += 3;
                        node->getChildren()->at(3)->setRelationalExpression(true);
                        node->getChildren()->at(3)->add_to_truelist(label1);
                        node->getChildren()->at(3)->add_to_falselist(label2);
                        node->getChildren()->at(3)->setCode("");
                    }
                }
                if (i == 6)
                {
                    if (expressionStack.size() != 0)
                    {
                        expressionStack.pop_back();
                        tempfile << generatePopCode("AX");
                    }
                }
            }
        }

        if (node->getRule().find("compound_statement : LCURL statements M RCURL") != string::npos)
        {
            backPatch(node->getChildren()->at(1)->get_nextlist(), node->getChildren()->at(2)->getNextLevel());
            node->set_nextlist(node->getChildren()->at(1)->get_nextlist());
        }

        if (node->getRule().find("compound_statement : LCURL RCURL") != string::npos)
        {
        }
        if (node->getRule().find("statements : statements M statement") != string::npos)
        {
            // node->sd="print this label";
            backPatch(node->getChildren()->at(0)->get_nextlist(), node->getChildren()->at(1)->getNextLevel());
            node->set_nextlist(node->getChildren()->at(2)->get_nextlist());
            // cout << "nextlist: here " << node->get_nextlist().size() << endl;
        }
        if (node->getRule().find("statements : statement-") != string::npos)
        {
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
        }
        if (node->getRule().find("statement : var declaration") != string::npos)
        {
        }
        if (node->getRule().find("statement : expression_statement") != string::npos)
        {
            if (expressionStack.size() != 0)
            {
                expressionStack.pop_back();
            }
            tempfile << generatePopCode("AX");
            lineNo++;
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
        }

        if (node->getRule().find("statement : compound_statement") != string::npos)
        {
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
        }

        if (node->getRule().find("statement : FOR LPAREN expression_statement M expression_statement M expression N RPAREN M statement N") != string::npos)
        {
            // cout<<"M1: "<<node->getChildren()->at(3)->getNextLevel()<<endl;
            // cout<<"M2: "<<node->getChildren()->at(5)->getNextLevel()<<endl;
            // cout<<"M3: "<<node->getChildren()->at(9)->getNextLevel()<<endl;
            // printList(node->getChildren()->at(3)->get_nextlist(),"nextlist1");
            // printList(node->getChildren()->at(11)->get_nextlist(),"nextlist2");
            // printList(node->getChildren()->at(9)->get_nextlist(),"nextlist3");
            node->set_nextlist(node->getChildren()->at(4)->get_falselist());

            backPatch(node->getChildren()->at(2)->get_nextlist(), node->getChildren()->at(3)->getNextLevel());
            backPatch(node->getChildren()->at(4)->get_truelist(), node->getChildren()->at(9)->getNextLevel());
            backPatch(node->getChildren()->at(11)->get_nextlist(), node->getChildren()->at(5)->getNextLevel());
            backPatch(node->getChildren()->at(7)->get_nextlist(), node->getChildren()->at(3)->getNextLevel());
            backPatch(node->getChildren()->at(9)->get_nextlist(), node->getChildren()->at(3)->getNextLevel());
            backPatch(node->getChildren()->at(10)->get_nextlist(), node->getChildren()->at(5)->getNextLevel());

            cout << "here" << endl;
        }
        if (node->getRule().find("statement : IF LPAREN expression RPAREN M statement-") != string::npos)
        {
            backPatch(node->getChildren()->at(2)->get_truelist(), node->getChildren()->at(4)->getNextLevel());
            node->set_nextlist(mergeList(node->getChildren()->at(2)->get_falselist(), node->getChildren()->at(4)->get_nextlist()));
            // cout << "nextlist: " << node->get_nextlist().size() << endl;
            // string str=node->get_nextlist().back();
            // cout<<"nextList:"<< str<<endl;
        }
        if (node->getRule().find("statement : IF LPAREN expression RPAREN M statement ELSE N M statement") != string::npos)
        {
            // cout << "here2" << endl;
            backPatch(node->getChildren()->at(2)->get_truelist(), node->getChildren()->at(4)->getNextLevel());
            backPatch(node->getChildren()->at(2)->get_falselist(), node->getChildren()->at(8)->getNextLevel());
            vector<string> *mergeL = mergeList(node->getChildren()->at(5)->get_nextlist(), node->getChildren()->at(7)->get_nextlist());
            // cout<<"nextlist3: " <<mergeL->size()<<endl;
            node->set_nextlist(mergeList(mergeL, node->getChildren()->at(9)->get_nextlist()));
            // for(int i=0;i<node->get_nextlist()->size();i++)
            // {
            //     cout<<"getlist"<<node->get_nextlist()->at(i)<<endl;
            // }
        }

        if (node->getRule().find("statement : WHILE M LPAREN expression RPAREN M statement") != string::npos)
        {

            backPatch(node->getChildren()->at(6)->get_nextlist(), node->getChildren()->at(1)->getNextLevel());
            backPatch(node->getChildren()->at(3)->get_truelist(), node->getChildren()->at(5)->getNextLevel());
            node->set_nextlist(node->getChildren()->at(3)->get_falselist());
            strings.clear();
            customSplit(node->getChildren()->at(1)->getNextLevel(), ':');
            string str = strings.at(0);
            strings.clear();
            tempfile << generateJump(str);
        }

        if (node->getRule().find("statement : PRINTLN LPAREN ID RPAREN SEMICOLON") != string::npos)
        {
            tempfile << generateMoveCode("AX", node->getChildren()->at(2)->getExp().id, to_string(node->getLineBegin()));
            tempfile << "\tCALL print_output" << endl;
            tempfile << "\tCALL new_line" << endl;
            lineNo += 3;
        }

        if (node->getRule().find("statement : PRINTLN LPAREN expression RPAREN SEMICOLON") != string::npos)
        {
        }
        if (node->getRule().find("statement : RETURN expression SEMICOLON N") != string::npos)
        {
            functionEnd = mergeList(node->getChildren()->at(3)->get_nextlist(), functionEnd);
        }
        if (node->getRule().find("expression_statement : expression SEMICOLON") != string::npos)
        {
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
        }
        if (node->getRule().find("expression_statement : SEMICOLON") != string::npos)
        {
        }

        if (node->getRule().find("variable : ID-") != string::npos)
        {
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
            node->setExp(node->getChildren()->at(0)->getExp());
        }
        if (node->getRule().find("variable : ID LSQUARE expression RSQUARE") != string::npos)
        {
            // tempfile<<node->getChildren()->at(2)->getCode();
            string code = "";
            // if(expressionStack.size()!=0)
            // {
            //     expressionStack.pop_back();
            //     code+=generatePopCode("AX",to_string(node->getLineBegin()));
            //     lineNo++;
            // }
            // code+=generatePushCode("AX",to_string(node->getLineBegin()));
            code += generatePopCode("BX");
            lineNo += 2;
            string id = node->getChildren()->at(0)->getExp().id;
            int offset = node->getChildren()->at(0)->offset;
            string arrayType = node->getChildren()->at(0)->arrayType;
            node->arrayType = arrayType;
            node->offset = offset;
            node->setExp2(id, "ID");
            if (arrayType == "local")
            {
                // cout<<"local"<<id;
                lineNo += 9;
                code += generateCodeForLocalArray(to_string(offset), to_string(node->getLineBegin()));
                code += generateMoveCode("AX", "[BP+SI]");
            }
            else
            {
                lineNo += 5;
                code += generateCodeForGlobalArray(to_string(node->getLineBegin()));
                code += generateMoveCode("AX", id + "[BX]");
            }
            node->setArrayCode(code);
            node->setCode(node->getChildren()->at(2)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
            node->setExp(node->getChildren()->at(0)->getExp());
        }
        if (node->getRule().find("expression : logic_expression") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
        }
        if (node->getRule().find("expression : variable ASSIGNOP logic_expression") != string::npos)
        {
            node->setPushed(node->getChildren()->at(2)->getPushed());
            bool isRelationalExp = node->getChildren()->at(2)->getRelationalExpression();
            if (isRelationalExp)
            {

                string Label1 = getNewLabel();
                string Label2 = getNewLabel();
                string Label3 = getNewLabel();
                tempfile << Label1 << endl;
                tempfile << generateMoveCode("AX", "1", to_string(node->getLineBegin()));
                strings.clear();
                customSplit(Label2, ':');
                string str = strings.at(0);
                strings.clear();
                tempfile << generateJump(str);
                tempfile << Label3 << endl;
                tempfile << generateMoveCode("AX", "0");
                tempfile << Label2 << endl;
                lineNo += 6;
                string trueLevel = Label1;
                string falseLevel = Label3;
                backPatch(node->getChildren()->at(2)->get_truelist(), trueLevel);
                backPatch(node->getChildren()->at(2)->get_falselist(), falseLevel);
                node->setPushed(false);
            }
            if (expressionStack.size() != 0)
            {
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                expressionStack.pop_back();
                lineNo++;
            }
            else
            {
                tempfile << node->getChildren()->at(2)->getCode();
                lineNo++;
            }
            if (node->getChildren()->at(2)->getArrayCode() != "")
            {
                tempfile << "\tPUSH AX" << endl;
                tempfile << node->getChildren()->at(2)->getArrayCode() << endl;
                // cout<<node->getChildren()->at(0)->getArrayCode()<<endl;
            }
            if (node->getChildren()->at(0)->getArrayCode() != "")
            {

                string id = node->getChildren()->at(0)->getExp().id;
                int offset = node->getChildren()->at(0)->offset;
                string arrayType = node->getChildren()->at(0)->arrayType;
                if (arrayType == "local")
                {
                    lineNo += 9;
                    tempfile << generateCodeLocalArrayExp(to_string(offset), to_string(node->getLineBegin()));
                    tempfile << generateMoveCode("[BP+SI]", "AX");
                }
                else
                {
                    tempfile << generateCodeForGlobalArrayExp(to_string(node->getLineBegin()));
                    tempfile << generateMoveCode(id + "[BX]", "AX");
                }
            }
            else
                tempfile << generateMoveCode(node->getChildren()->at(0)->getExp().id, "AX");
            lineNo++;
            tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
            expressionStack.push_back("AX");
            lineNo++;
        }
        if (node->getRule().find("logic_expression : rel_expression-") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
        }
        if (node->getRule().find("logic_expression : rel_expression LOGICOP M rel_expression") != string::npos)
        {
            if (!node->getChildren()->at(3)->getRelationalExpression())
            {
                string str = node->getChildren()->at(3)->getCode();
                if (str != "")
                {
                    tempfile << str;
                    lineNo++;
                }
                else
                {
                    tempfile << generatePopCode("AX");
                    expressionStack.pop_back();
                    lineNo++;
                }
                tempfile << generateCMP("AX", "0");
                string label1 = getNewDummyLabel();
                string label2 = getNewDummyLabel();
                tempfile << generateJMP("!=", label1, label2);
                lineNo += 3;
                node->getChildren()->at(3)->setRelationalExpression(true);
                node->getChildren()->at(3)->add_to_truelist(label1);
                node->getChildren()->at(3)->add_to_falselist(label2);
                node->getChildren()->at(3)->setCode("");
            }
            string str = node->getChildren()->at(1)->getCode();
            //  cout<<"str: "<<str<<endl;
            //  cout<<node->getChildren()->at(0)->get_truelist().size()<<endl;
            //  cout<<node->getChildren()->at(0)->get_falselist().size()<<endl;
            //  cout<<node->getChildren()->at(2)->get_truelist().size()<<endl;
            //  cout<<node->getChildren()->at(2)->get_falselist().size()<<endl;
            if (str.find("||") != string::npos)
            {
                backPatch(node->getChildren()->at(0)->get_falselist(), node->getChildren()->at(2)->getNextLevel());
                node->set_truelist(mergeList(node->getChildren()->at(0)->get_truelist(), node->getChildren()->at(3)->get_truelist()));
                node->set_falselist(node->getChildren()->at(3)->get_falselist());
            }
            else if (str.find("&&") != string::npos)
            {
                backPatch(node->getChildren()->at(0)->get_truelist(), node->getChildren()->at(2)->getNextLevel());
                node->set_falselist(mergeList(node->getChildren()->at(0)->get_falselist(), node->getChildren()->at(3)->get_falselist()));
                node->set_truelist(node->getChildren()->at(3)->get_truelist());
            }
            node->setRelationalExpression(true);
        }
        if (node->getRule().find("rel_expression : simple_expression-") != string::npos)
        {
            cout << "here" << endl;
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
            simpleExpression = true;
        }
        if (node->getRule().find("rel_expression : simple_expression RELOP simple_expression") != string::npos)
        {
            node->relationalExpressionEvaluated = true;
            if (node->getChildren()->at(2)->getCode() != "" && node->getChildren()->at(0)->getCode() != "")
            {
                tempfile << node->getChildren()->at(2)->getCode();
                tempfile << generateMoveCode("DX", "AX");
                tempfile << node->getChildren()->at(0)->getCode();
                lineNo += 1;
            }
            else if (node->getChildren()->at(2)->getCode() != "" || node->getChildren()->at(0)->getCode() != "")
            {
                if (node->getChildren()->at(0)->getCode() != "")
                {
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                }
                else
                {
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                }
                expressionStack.pop_back();
                lineNo += 2;
            }
            else if (node->getChildren()->at(2)->getCode() == "" && node->getChildren()->at(0)->getCode() == "")
            {
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                tempfile << generateMoveCode("DX", "AX");
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                lineNo += 3;
                expressionStack.pop_back();
                expressionStack.pop_back();
            }
            string str = node->getChildren()->at(1)->getCode();
            tempfile << generateCMP("AX", "DX");
            string label1 = getNewDummyLabel();
            string label2 = getNewDummyLabel();
            tempfile << generateJMP(str, label1, label2);
            lineNo += 3;
            node->setRelationalExpression(true);
            node->add_to_truelist(label1);
            node->add_to_falselist(label2);
            node->setCode("");
            node->relationalExpressionEvaluated = true;
            // tempfile<<endl;
            // tempfile<<"hello"<<endl;
        }
        //! ei porjonto array code push korchi...nice ;)
        if (node->getRule().find("simple_expression : term") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
            // backpatch code
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
        }
        if (node->getRule().find("simple_expression : simple_expression ADDOP term") != string::npos)
        {

            if (node->getChildren()->at(2)->getCode() != "" && node->getChildren()->at(0)->getCode() != "")
            {
                if (node->getChildren()->at(2)->getArrayCode() != "" && node->getChildren()->at(0)->getArrayCode() != "")
                {
                    // cout<<"array code"<<endl;
                    // cout<<node->getChildren()->at(2)->getArrayCode()<<endl;
                    tempfile << node->getChildren()->at(0)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(0)->getArrayCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(2)->getArrayCode();
                }
                else if (node->getChildren()->at(0)->getArrayCode() != "" && node->getChildren()->at(2)->getArrayCode() == "")
                {
                    tempfile << node->getChildren()->at(0)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(0)->getArrayCode();
                }
                else if (node->getChildren()->at(0)->getArrayCode() == "" && node->getChildren()->at(2)->getArrayCode() != "")
                {

                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getArrayCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                }
                else
                {
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                    lineNo += 1;
                }
            }
            else if (node->getChildren()->at(2)->getCode() != "" || node->getChildren()->at(0)->getCode() != "")
            {
                if (node->getChildren()->at(0)->getCode() != "")
                {
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                }
                else
                {
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("DX", "AX");
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                }
                expressionStack.pop_back();
                lineNo += 2;
            }
            else if (node->getChildren()->at(2)->getCode() == "" && node->getChildren()->at(0)->getCode() == "")
            {
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                tempfile << generateMoveCode("DX", "AX");
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                lineNo += 3;
                expressionStack.pop_back();
                expressionStack.pop_back();
            }

            if (node->getChildren()->at(1)->getCode().find("+") != string::npos)
                tempfile << generateAddCode("AX", "DX");
            else
                tempfile << generateSubCode("AX", "DX");
            tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
            expressionStack.push_back("AX");
            lineNo += 2;
            node->setPushed(true);
        }
        if (node->getRule().find("term : unary_expression") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());

            // backpatch code
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
        }

        if (node->getRule().find("term : term MULOP unary_expression") != string::npos)
        {
            if (node->getChildren()->at(2)->getCode() != "" && node->getChildren()->at(0)->getCode() != "")
            {
                if (node->getChildren()->at(2)->getArrayCode() != "" && node->getChildren()->at(0)->getArrayCode() != "")
                {
                    cout << "array code" << endl;
                    tempfile << node->getChildren()->at(0)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(0)->getArrayCode();
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << node->getChildren()->at(2)->getArrayCode();
                }
                else if (node->getChildren()->at(0)->getArrayCode() != "" && node->getChildren()->at(2)->getArrayCode() == "")
                {
                    tempfile << node->getChildren()->at(0)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << node->getChildren()->at(0)->getArrayCode();
                }
                else if (node->getChildren()->at(0)->getArrayCode() == "" && node->getChildren()->at(2)->getArrayCode() != "")
                {

                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
                    tempfile << node->getChildren()->at(2)->getArrayCode();
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                }
                else
                {
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                    lineNo += 1;
                }
            }
            else if (node->getChildren()->at(2)->getCode() != "" || node->getChildren()->at(0)->getCode() != "")
            {
                if (node->getChildren()->at(0)->getCode() != "")
                {
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << node->getChildren()->at(0)->getCode();
                }
                else
                {
                    tempfile << node->getChildren()->at(2)->getCode();
                    tempfile << generateMoveCode("CX", "AX");
                    tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                }
                expressionStack.pop_back();
                lineNo += 2;
            }
            else if (node->getChildren()->at(2)->getCode() == "" && node->getChildren()->at(0)->getCode() == "")
            {
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                tempfile << generateMoveCode("CX", "AX");
                tempfile << generatePopCode("AX", to_string(node->getLineBegin()));
                lineNo += 3;
                expressionStack.pop_back();
                expressionStack.pop_back();
            }
            if (node->getChildren()->at(1)->getCode().find("*") != string::npos)
                tempfile << generateMulCode("CX");
            else
                tempfile << generateDivCode("CX");
            if (node->getChildren()->at(1)->getCode().find("%") != string::npos)
                tempfile << generatePushCode("DX", to_string(node->getLineBegin()));
            else
                tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
            expressionStack.push_back("AX");
            node->setPushed(true);
            lineNo += 3;
        }

        if (node->getRule().find("unary_expression : ADDOP unary_expression") != string::npos)
        {
            string code = "";
            code += node->getChildren()->at(1)->getCode();
            if (node->getChildren()->at(0)->getCode().find("-") != string::npos)
            {

                code += generateNegCode("AX");
                code += generatePushCode("AX", to_string(node->getLineBegin()));
                lineNo += 2;
                node->setPushed(true);
                expressionStack.push_back("AX");
                tempfile << code;
                code = "";
            }
            node->setCode(code);
        }
        if (node->getRule().find("unary_expression : NOT unary_expression") != string::npos)
        {
            if (!node->getChildren()->at(1)->getRelationalExpression())
            {
                string code = "";
                code += node->getChildren()->at(1)->getCode();
                code += generateNotCode("AX");
                code += generatePushCode("AX", to_string(node->getLineBegin()));
                lineNo += 2;
                node->setPushed(true);
                expressionStack.push_back("AX");
                tempfile << code;
                node->setCode("");
            }
            else
            {
                node->set_truelist(node->getChildren()->at(1)->get_falselist());
                node->set_falselist(node->getChildren()->at(1)->get_truelist());
                node->set_nextlist(node->getChildren()->at(1)->get_nextlist());
                node->setRelationalExpression(node->getChildren()->at(1)->getRelationalExpression());
            }
        }
        if (node->getRule().find("unary_expression : factor") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());

            // backpatch code
            node->set_truelist(node->getChildren()->at(0)->get_truelist());
            node->set_falselist(node->getChildren()->at(0)->get_falselist());
            node->set_nextlist(node->getChildren()->at(0)->get_nextlist());
            node->setRelationalExpression(node->getChildren()->at(0)->getRelationalExpression());
        }
        if (node->getRule().find("factor : variable-") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
        }

        if (node->getRule().find("factor : CONST_INT") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(0)->getArrayCode());
            node->setCode(node->getChildren()->at(0)->getCode());
            node->setPushed(node->getChildren()->at(0)->getPushed());
        }
        if (node->getRule().find("factor : ID LPAREN argument_list RPAREN") != string::npos)
        {
            tempfile << "\tCALL " << node->getChildren()->at(0)->getExp().id << endl;
            tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
            expressionStack.push_back("AX");
        }
        if (node->getRule().find("factor : LPAREN expression RPAREN") != string::npos)
        {
            node->setArrayCode(node->getChildren()->at(1)->getArrayCode());
            node->setCode(node->getChildren()->at(1)->getCode());
            node->setPushed(node->getChildren()->at(1)->getPushed());

            // backpatch code
            node->set_truelist(node->getChildren()->at(1)->get_truelist());
            node->set_falselist(node->getChildren()->at(1)->get_falselist());
            node->set_nextlist(node->getChildren()->at(1)->get_nextlist());
            node->setRelationalExpression(node->getChildren()->at(1)->getRelationalExpression());
        }
        if (node->getRule().find("factor : variable INCOP") != string::npos)
        {
            string code;
            code += node->getChildren()->at(0)->getCode();
            code += generatePushCode("AX", to_string(node->getLineBegin()));
            code += generateIncCode("AX");
            string str = node->getChildren()->at(0)->getExp().id;
            code += generateMoveCode(str, "AX");
            tempfile << code;
            node->setCode("");
            node->setPushed(true);
            expressionStack.push_back("AX");
            lineNo += 3;
            incFlag = true;
        }
        if (node->getRule().find("factor : variable DECOP") != string::npos)
        {
            string code;
            code += node->getChildren()->at(0)->getCode();
            code += generatePushCode("AX", to_string(node->getLineBegin()));
            code += generateDecCode("AX");
            string str = node->getChildren()->at(0)->getExp().id;
            code += generateMoveCode(str, "AX");
            tempfile << code;
            node->setCode("");
            node->setPushed(true);
            lineNo += 3;
            expressionStack.push_back("AX");
        }

        if (node->getRule().find("argument list : arguments") != string::npos)
        {
        }
        if (node->getRule().find("arguments : arguments COMMA logic_expression") != string::npos)
        {
            tempfile << node->getChildren()->at(2)->getCode();
            tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
        }
        if (node->getRule().find("arguments : logic_expression") != string::npos)
        {
            tempfile << node->getChildren()->at(0)->getCode();
            tempfile << generatePushCode("AX", to_string(node->getLineBegin()));
        }
    }

    //////////////////////!!!!!!!!!
    //////////////////////!!!!!!!!!
    //////////////////////!!!!!!!!!
    //////////////////////!!!!!!!!!

    void printLocalVar(ParseTree *node)
    {

        if (node->getIsTerminal())
        {
            return;
        }
        for (int i = 0; i < node->getChildren()->size(); i++)
        {
            if (node->getChildren()->at(i)->getRule().find("ID :") != string::npos)
            {
                int count = node->getChildren()->size() - i;
                if (count > 1)
                {
                    strings.clear();
                    customSplit(node->getChildren()->at(i)->getRule(), ' ');
                    string str = strings.at(2);
                    currentLocalVar = str;
                    SymbolInfo *symbol = new SymbolInfo(currentLocalVar, "ID", "INT", "ARRAY", "");
                    string str2 = node->getChildren()->at(i + 2)->getRule();
                    strings.clear();
                    customSplit(str2, ':');
                    str2 = strings.at(1);
                    symbol->setArrayOffset(stoi(str2));
                    totalOffset += stoi(str2) * 2;
                    symbol->setOffset(totalOffset);
                    bool flag = tempTable->insert(symbol);

                    if (flag)
                    {
                        tempfile << "\t"
                                 << "SUB SP, " << stoi(str2) * 2 << endl;
                        lineNo++;
                    }
                }
                else
                {
                    strings.clear();
                    customSplit(node->getChildren()->at(i)->getRule(), ' ');
                    string str = strings.at(2);
                    currentLocalVar = str;
                    SymbolInfo *symbol = new SymbolInfo(currentLocalVar, "ID", "INT", "VARIABLE", "");
                    totalOffset += 2;
                    symbol->setOffset(totalOffset);
                    bool flag = tempTable->insert(symbol);

                    strings.clear();
                    if (flag)
                    {
                        tempfile << "\t"
                                 << "SUB SP, 2" << endl;
                        lineNo++;
                    }
                }
            }
            printLocalVar(node->getChildren()->at(i));
        }
    }
};
