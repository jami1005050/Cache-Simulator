/*Date: 03/03/2020
Class: CS5541
Assignment: Cache Simulator
Author(s): Mohammad Jaminur Islam*/

There is only one file csimulator.c
I used visual stduio code for developing it. But I compiled it from the command line and also tested it on the cs server. There is
no warning and error.
The program can be compile and run by the following command.
below I have used -lm option to link the math library. 

gcc -std=c99 -Wall -o csim csimulator.c -lm
example command for trace files.
 ./csim -s 5 -E 1 -b 5 -t traces/long.trace

I took help from the following links to implement the assignment.
https://www.rapidtables.com/convert/number/decimal-to-binary.html
https://stackoverflow.com/questions/2844/how-do-you-format-an-unsigned-long-long-int-using-printf

https://stackoverflow.com/questions/12824134/undefined-reference-to-pow-in-c-despite-including-math-h/12824148
https://cs.stackexchange.com/questions/33730/cache-direct-map-index-tag-hit-miss