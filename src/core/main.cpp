/***********************************************
 *
 * File: main.cpp
 * Author: Yarygin Alexander <zpp0@mail.ru>
 *
 ***********************************************/

#include <QtCore/QCoreApplication>
#include <iostream>

#include "simulator.h"

int main(int argc, char *argv[])
{
    qDebug() << "args" << argv[0] << argv[1] << argv[2];
    if (argc > 1) {
        simulator* sim;
        if (argc > 2) {
            if (strcmp(argv[2], "binary") == 0)
                sim = new simulator(argv[1], log::BINARY);
            else if (strcmp(argv[2], "text") == 0)
                sim = new simulator(argv[1], log::TEXT);
            else {
                std::cerr << "Warning: format arg is out. Use binary format by defalut." << std::endl;
                sim = new simulator(argv[1], log::BINARY);
            }
        }
        else {
            std::cerr << "Warning: format arg is out. Use binary format by defalut." << std::endl;
            sim = new simulator(argv[1], log::BINARY);
        }
            
        sim->eval();
        delete sim;
    }
    else
        std::cerr << "ERROR: Project file arg is out. Exit." << std::endl;
    
    return 0;
}
