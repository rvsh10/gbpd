# test_task


Before you get started, you need to check the availability of the necessary dependencies for the program to work:
  - GNU Compiler Collection (to compile source code)
  - curl (to send requests)
  - nlohmann-json (for correct display and processing of data)

Installation:

Open console and enter the following commands:
  
  Clone repository:
  - git clone https://github.com/rvsh10/test_task && cd test_task/src/

Install shared library:
  - sudo mv libgetbranchdata.so.1.0.0 /usr/lib/
  - sudo ldconfig 

Compile cli.cpp:
  - g++ cli.cpp -o gbd /usr/lib/libgetbranchdata.so.1.0.0 -lcurl

(you can specify any other name instead of "gbd")

Install the utility into the system:
  - sudo mv gdb /usr/bin

Usage:

To use utility open console type gdb (or the name you specified) and specify arguments:

gdb branch_1 branch_2


