
//
// Created by 23207 on 2020/10/4.
//

#ifndef COMPILER_GRAMMAR_EXCEPTION_H
#define COMPILER_GRAMMAR_EXCEPTION_H
#include <exception>
using namespace std;


class grammar_exception : public exception{
public:
    const char* what () const throw ()
    {
        return "Grammar Exception";
    }
};


#endif //COMPILER_GRAMMAR_EXCEPTION_H
