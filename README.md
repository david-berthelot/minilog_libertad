minilog_libertad
================

Simple netlist verilog reader and .LIB format reader.

This source code provides two basic libraries:
- MINILOG is a simple verilog netlist parser
- LIBERTAD is a simple .LIB parser


Requirements:
---------------
You must have Bison, Flex and Doxygen installed.


Documentation:
--------------
The documentation is autogenerated using Doxygen, type 
```bash
make Docs
```

After running the previous commands, the docs can be found in
- LIBERTAD/DOC
- MINILOG/DOC


Compilation:
------------
To compile the libraries and sample applications, type
```bash
make All
```

This will produce 3 sample executable files demo-ing the APIs.
- LIBERTAD/EXAMPLES/libreader.exe
- MINILOG/EXAMPLES/vlogreader.exe
- EXAMPLES/netlib.exe

Three libraries will be produced too:
- UTILS/LIB/*/libutil.a
- LIBERTAD/LIB/*/liblibertad.a
- MINILOG/LIB/*/libminilog.a


Licence: MIT-Licence
--------
Copyright (c) 2013 David Berthelot

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
