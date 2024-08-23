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
  - sudo mv libgbpd.so.1.0.3 /usr/lib/
  - sudo ldconfig 

Compile cli.cpp:
  - g++ cli.cpp -o gbpd /usr/lib/libgbpd.so.1.0.3 -lcurl

(you can specify any other name instead of "gbpd")

Install the utility into the system:
  - sudo mv gbpd /usr/bin

Usage:

To use utility open console type gbpd (or the name you specified) and specify arguments:

gbpd branch_1 branch_2


