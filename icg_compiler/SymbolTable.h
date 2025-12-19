#pragma once
#include "ScopeTable.h"
#include <fstream>

class SymbolTable {
private:
    ScopeTable* currentScopeTable;
    int bucketSize = 2;
    int currentSIDDeletedScopes = 1;

public:
    SymbolTable(int bucketSize)
        : bucketSize(bucketSize), currentScopeTable(new ScopeTable(bucketSize)) {
        currentScopeTable->setUniqueId("1");
    }

    ~SymbolTable() {
        while (currentScopeTable != nullptr) {
            ScopeTable* parentScopeTable = currentScopeTable->getParentScope();
            delete currentScopeTable;
            currentScopeTable = parentScopeTable;
        }
    }

    void enterScope() {
        ScopeTable* newScopeTable = new ScopeTable(bucketSize);
        newScopeTable->setParentScope(currentScopeTable);
        string uniqueId = currentScopeTable->getUniqueId();
        currentScopeTable = newScopeTable;
        string newUniqueId = uniqueId + "." + to_string(currentSIDDeletedScopes);
        currentScopeTable->setUniqueId(newUniqueId);
        currentSIDDeletedScopes = 1;
    }

    bool insert(SymbolInfo* symbol) {
        return currentScopeTable->insert(symbol);
    }

    void  parentScopeTableInsert(SymbolInfo* symbol) {
        if(currentScopeTable->getParentScope() != nullptr)
         currentScopeTable->getParentScope()->insert(symbol);
    }

    SymbolInfo* lookup(string key) {
        ScopeTable* tempScopeTable = currentScopeTable;
        SymbolInfo* symbol = nullptr;
        while (tempScopeTable != nullptr) {
            symbol = tempScopeTable->lookup(key);
            if (symbol != nullptr) {
                return symbol;
            }
            tempScopeTable = tempScopeTable->getParentScope();
        }
        return symbol;
    }

    SymbolInfo* lookupCurrent(string key) {
        return currentScopeTable->lookup(key);
    }

    void printCurrent(ofstream& file) {
        currentScopeTable->print(file);
    }


    void printAll(ofstream& file) {
        ScopeTable* tempScopeTable = currentScopeTable;
        while (tempScopeTable != nullptr) {
            tempScopeTable->print(file);
            tempScopeTable = tempScopeTable->getParentScope();
        }
    }

    void remove(string key) {
        currentScopeTable->deleteSymbol(key);
    }

    bool exitScope() {
        if (currentScopeTable->getParentScope() == nullptr) {
            return false;
        } else {
            ScopeTable* tempScopeTable = currentScopeTable;
            currentScopeTable = currentScopeTable->getParentScope();
            string id = tempScopeTable->getUniqueId();
            istringstream iss(id);
            string component;
            int lastNumber = 0;
            while (getline(iss, component, '.')) {
                int number;
                istringstream(component) >> number;
                lastNumber = number;
            }

            delete tempScopeTable;
            currentSIDDeletedScopes = lastNumber + 1;
            return true;
        }
    }
};

// SYMBOLTABLE_H
