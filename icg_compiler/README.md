# ICG Compiler

This repository contains an Intermediate Code Generator (ICG) based C-like compiler project.

## Overview

- Components: lexer, parser, code generator, symbol tables, and other helpers.
- Files of interest: `lexer.l`, `parser.y`, `CodeGenerator.h`, `AssemblyGenerator.h`.

## Build / Test

This project uses classic lex/yacc (flex/bison) style tooling. Typical build steps:

1. Generate lexer and parser:

   - `flex lexer.l`
   - `bison -d parser.y`

2. Compile generated and supporting sources with `gcc`