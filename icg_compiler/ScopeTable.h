#pragma once
#include "SymbolInfo.h"
#include <bits/stdc++.h>
using namespace std;

class ScopeTable
{
private:
    SymbolInfo **table;
    ScopeTable *parentScope;
    int bucketSize;
    string uniqueId;

public:
    ScopeTable(int bucketSize)
        : bucketSize(bucketSize), table(new SymbolInfo *[bucketSize]), parentScope(nullptr)
    {
        for (int i = 0; i < bucketSize; i++)
        {
            table[i] = nullptr;
        }
    }

    ~ScopeTable()
    {
        for (int i = 0; i < bucketSize; i++)
        {
            delete table[i];
        }
        delete[] table;
    }

    std::string getUniqueId()
    {
        return uniqueId;
    }

    void setUniqueId(string uniqueId)
    {
        this->uniqueId = uniqueId;
    }

    ScopeTable *getParentScope()
    {
        return parentScope;
    }

    void setParentScope(ScopeTable *parentScope)
    {
        this->parentScope = parentScope;
    }

    int getBucketSize()
    {
        return bucketSize;
    }

    unsigned long long hashFunction(string str)
    {
        unsigned long long hash = 0;
        for (auto ch : str)
        {
            int c = ch;
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        hash = hash % getBucketSize();
        return hash;
    }

    bool insert(SymbolInfo *symbol)
    {
        unsigned long long hash = hashFunction(symbol->getName());
        if (table[hash] == nullptr)
        {
            table[hash] = symbol;
            return true;
        }
        else
        {
            SymbolInfo *temp = table[hash];
            while (temp->getNext() != nullptr)
            {
                if (temp->getName() == symbol->getName())
                {
                    return false;
                }
                temp = temp->getNext();
            }
            if (temp->getName() == symbol->getName())
            {
                return false;
            }
            temp->setNext(symbol);
            return true;
        }
    }

    SymbolInfo *lookup(string name)
    {
        unsigned long hash = hashFunction(name);
        if (table[hash] == nullptr)
        {
            return nullptr;
        }
        else
        {
            SymbolInfo *temp = table[hash];
            while (temp != nullptr)
            {
                if (temp->getName() == name)
                {
                    return temp;
                }
                temp = temp->getNext();
            }
            return temp;
        }
    }

    bool deleteSymbol(string name)
    {
        unsigned long hash = hashFunction(name);
        if (table[hash] == nullptr)
        {
            return false;
        }
        else
        {
            SymbolInfo *temp = table[hash];
            SymbolInfo *prev = nullptr;
            while (temp != nullptr)
            {
                if (temp->getName() == name)
                {
                    if (prev == nullptr)
                    {
                        table[hash] = temp->getNext();
                        delete temp;
                        return true;
                    }
                    else
                    {
                        prev->setNext(temp->getNext());
                        delete temp;
                        return true;
                    }
                }
                prev = temp;
                temp = temp->getNext();
            }
            return false;
        }
    }

    void print(std::ofstream &outFile)
    {
        outFile << "\tScopeTable# " << getUniqueId() << "\n";

        for (int i = 1; i <= getBucketSize(); i++)
        {
            SymbolInfo *cur = table[i - 1];
            if (cur == nullptr)
            {
                continue;
            }
            if (cur->getName() == "")
            {
                continue;
            }
            outFile << "\t" << i;
            if (cur->getSymbolType() == "VARIABLE")
            {
                outFile << "--> <" << cur->getName() << "," << cur->getDataType() << "> ";
            }
            else if (cur->getSymbolType() == "ARRAY")
            {
                outFile << "--> <" << cur->getName() << "," << cur->getSymbolType() << "> ";
            }
            else
            {
                outFile << "--> <" << cur->getName() << "," << cur->getSymbolType() << "," << cur->getDataType() << "> ";
            }
            cur = cur->getNext();
            while (cur != nullptr && cur->getName() != "")
            {
                if (cur->getSymbolType() == "VARIABLE")
                {
                    outFile << "<" << cur->getName() << "," << cur->getDataType() << "> ";
                }
                else if (cur->getSymbolType() == "ARRAY")
                {
                    outFile << "<" << cur->getName() << "," << cur->getSymbolType() << "> ";
                }
                else
                {
                    outFile << "<" << cur->getName() << "," << cur->getSymbolType() << "," << cur->getDataType() << "> ";
                }
                cur = cur->getNext();
            }
            outFile << "\n";
        }
    }
};
