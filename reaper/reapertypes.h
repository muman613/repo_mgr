#ifndef __REAPERTYPES_H__
#define __REAPERTYPES_H__

#include <string>
#include <vector>

typedef std::string                                 STRING;
typedef std::vector<STRING>                         STRING_VECTOR;
typedef std::vector<STRING>::iterator               STRING_VECTOR_ITER;
typedef std::pair<STRING,STRING>                    STRING_PAIR;
typedef std::vector<STRING_PAIR>                    STRING_PAIR_VECTOR;
typedef std::vector<STRING_PAIR>::iterator          STRING_PAIR_VECTOR_ITER;


#endif // __REAPERTYPES_H__
