#pragma once
#include <bits/stdc++.h>
using namespace std;
string generateMoveCode(string source, string destination, string lineno = "move")
{
    string str;
    if (lineno .find("move")==string::npos)
        str = "\tMOV " + source + ", " + destination + "       ; Line " + lineno + "\n";
    else
        str = "\tMOV " + source + ", " + destination + "\n";
    return str;
}

string generateAddCode(string source, string destination)
{
    string str = "\tADD " + source + ", " + destination + "\n";
    return str;
}
string generateSubCode(string source, string destination)
{
    string str = "\tSUB " + source + ", " + destination + "\n";
    return str;
}

string generatePushCode(string source,string lineno)
{
    string str = "\tPUSH " + source + "\n";
    return str;
}

string generatePopCode(string destination,string lineno = "pop")
{
    string str;
    if (lineno.find("pop")==string::npos)
        str = "\tPOP " + destination + "       ; Line " + lineno + "\n";
    else
        str = "\tPOP " + destination + "\n";
    return str;
}

string generateNegCode(string source)
{
    string str = "\tNEG " + source + "\n";
    return str;
}

string generateIncCode(string source)
{
    string str = "\tINC " + source + "\n";
    return str;
}
string generateDecCode(string source)
{
    string str = "\tDEC " + source + "\n";
    return str;
}
string generateDivCode(string source)
{
    string str = "\tCWD\n\tDIV " + source + "\n";
    return str;
}
string generateMulCode(string source)
{
    string str = "\tCWD\n\tMUL " + source + "\n";
    return str;
}
string generateCMP(string source, string destination)
{
    string str = "\tCMP " + source + ", " + destination + "\n";
    return str;
}

string generateJump(string label)
{
    string str = "\tJMP " + label + "\n";
    return str;
}
string generateJMP(string opcode,string label1,string label2)
{
    string str="";
    if(opcode.find("==")!= string::npos)
    {
        str += "\tJE " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    else if(opcode.find("!=")!= string::npos)
    {
        str += "\tJNE " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    else if(opcode.find(">=")!= string::npos)
    {
        str += "\tJGE " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    else if(opcode.find("<=")!= string::npos)
    {
        str += "\tJLE " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    else if(opcode.find(">")!= string::npos)
    {
        str += "\tJG " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    else if(opcode.find("<")!= string::npos)
    {
        str += "\tJL " + label1 + "\n";
        str += "\tJMP " + label2 + "\n";
    }
    return str;
}

string generateNotCode(string source)
{
    string str = "\tNOT " + source + "\n";
    return str;
}


string generateCodeLocalArrayExp(string offset,string lineno)
{
    string str;
    str+="\tPOP BX\n";
    str+="\tPUSH AX\n";
    str+="\tMOV AX, 2\n";
    str+="\tMUL BX\n";
    str+="\tMOV BX, AX\n";
    str+="\tMOV AX, "+offset+"\n";
    str+="\tSUB AX, BX\n";
    str+="\tMOV BX, AX\n";
    str+="\tPOP AX\n";
    str+="\tMOV SI, BX\n";
    str+="\tNEG SI\n";
    return str;

}
string generateCodeForLocalArray(string offset,string lineno )
{
    string str;
    str+="\tMOV AX, 2       ; Line "+lineno+"\n";
    str+="\tMUL BX\n";
    str+="\tMOV BX, AX\n";
    str+="\tMOV AX, "+offset+"\n";
    str+="\tSUB AX, BX\n";
    str+="\tMOV BX, AX\n";
    str+="\tMOV SI, BX\n";
    str+="\tNEG SI\n";
    return str;
}



string generateCodeForGlobalArray(string lineno)
{
    string str;
    str+="\tMOV AX, 2       ; Line "+lineno+"\n";
    str+="\tMUL BX\n";
    str+="\tMOV BX, AX\n";
    return str;

}

string generateCodeForGlobalArrayExp(string lineno)
{
    string str;
    str+="\tPOP BX\n";
    str+="\tPUSH AX\n";
    str+="\tMOV AX, 2\n";
    str+="\tMUL BX\n";
    str+="\tMOV BX, AX\n";
    str+="\tPOP AX\n";

    return str;

}



//array generation a jodi expression stack size >1 hoy then expression stack pop kora lagbe