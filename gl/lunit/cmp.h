#ifndef GL_LUNIT_CMP_H_
#define GL_LUNIT_CMP_H_

#include <stdbool.h>
#include <stddef.h>

bool ByteEq(char a, char b);
void ByteEqMsg(char* msg, char a, char b);

bool BoolEq(bool a, bool b);
void BoolEqMsg(char* msg, bool a, bool b);

bool IntEq(int a, int b);
void IntEqMsg(char* msg, int a, int b);

bool CharEq(char a, char b);
void CharEqMsg(char* msg, char a, char b);

bool StrEq(char* a, char* b);
bool StrNEq(char* a, char* b);
bool StrStartsWith(char* str, char* sub_str);
void StrEqMsg(char* msg, char* a, char* b);
void StrNEqMsg(char* msg, char* a, char* b);
void StrStartsWithMsg(char* msg, char* a, char* b);

bool MemEq(void* a, void* b, size_t size);
void MemEqMsg(char* msg, void* a, void* b, size_t size);

#endif // GL_LUNIT_CMP_H_
