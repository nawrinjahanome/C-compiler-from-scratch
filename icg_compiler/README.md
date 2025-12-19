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

2. Compile generated and supporting sources with `gcc`.

Adjust paths and commands to match your environment.


## License

See repository headers and project files for licensing details.
History update 2 - Fri Dec 19 01:26:38 AM UTC 2025
History update 3 - Fri Dec 19 01:26:39 AM UTC 2025
History update 4 - Fri Dec 19 01:26:39 AM UTC 2025
History update 5 - Fri Dec 19 01:26:40 AM UTC 2025
History update 6 - Fri Dec 19 01:26:40 AM UTC 2025
History update 7 - Fri Dec 19 01:26:41 AM UTC 2025
History update 8 - Fri Dec 19 01:26:41 AM UTC 2025
