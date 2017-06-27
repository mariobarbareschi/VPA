#include <iostream>
#include <unistd.h>
#include <string.h>

#include "vpa.h"

#define GENERATE_RAND_FLOAT           (float)(((double)rand()/RAND_MAX) * 100)
#define GENERATE_RAND_DOUBLE          (((double)rand()/RAND_MAX)*100)


void test(void);
void printUsage(char* name);

int main(int argc, char* argv[]){
    if(argc == 1 || argc == 2 || argc == 4 || argc > 7) printUsage(argv[0]);
    bool testMode = false;
    bool inputPassed = false;
    bool outputPassed = false;
    double inputValue;
    ::vpa::FloatingPointPrecision outPrecision;
    char opt;
    while ((opt = getopt(argc, argv, "hm:i:o:")) != -1) {
        switch (opt) {
        case 'm':
            if(optarg[0] == 't')
                testMode = true;
            else if(optarg[0] == 'c')
                testMode = false;
            break;
        case 'i':
            inputValue = atof(optarg);
            inputPassed = true;
            break;
        case 'o':
            outputPassed = true;
            if(0 == strcmp(optarg, "h"))
                outPrecision = ::vpa::half_prec;
            else if(0 == strcmp(optarg, "f"))
                outPrecision = ::vpa::float_prec;
            else if(0 == strcmp(optarg, "d"))
                outPrecision = ::vpa::double_prec;
            else{
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
        default: /* '?' */
            printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if(testMode) test();
    else{
        if(inputPassed && outputPassed){
            ::vpa::VPA valIn(inputValue);
            ::vpa::VPA valOut(inputValue, outPrecision);
            std::cout<< "Input value: " << valIn << std::endl;
            std::cout<< "Output value: " << valOut << std::endl;
        }
    }
    return 1;
}

void test(){
    /*The following code is used to check that the procedures used by the VPA class
are compliant with IEEE 754, indeed the result of every operation on two random float/double values
is the same with native float/double type and FloatingPointType objects*/
    std::cout << "Running testing..." << std::endl;
    long int k = 0;
    std::cout << "Cycle: ";
    std::cout.flush();
    while (1) {
        ::vpa::VPA::test(GENERATE_RAND_FLOAT, GENERATE_RAND_FLOAT);
        ::vpa::VPA::test(GENERATE_RAND_FLOAT, -GENERATE_RAND_FLOAT);
        ::vpa::VPA::test(-GENERATE_RAND_FLOAT, GENERATE_RAND_FLOAT);
        ::vpa::VPA::test(-GENERATE_RAND_FLOAT,
                                       -GENERATE_RAND_FLOAT);
        ::vpa::VPA::test(GENERATE_RAND_DOUBLE,
                                       GENERATE_RAND_DOUBLE);
        ::vpa::VPA::test(GENERATE_RAND_DOUBLE,
                                       -GENERATE_RAND_DOUBLE);
        ::vpa::VPA::test(-GENERATE_RAND_DOUBLE,
                                        GENERATE_RAND_DOUBLE);
        ::vpa::VPA::test(-GENERATE_RAND_DOUBLE,
                                      -GENERATE_RAND_DOUBLE);
        if(0 == (k % 1000)) std::cout << "\rCycle: " << k;
        k++;
        }

}

void printUsage(char* name){
    std::cout << "Usage: " << name << "-m [t|c] [-i value] [-o h|d|f]" << std::endl;
    std::cout.flush();
}
