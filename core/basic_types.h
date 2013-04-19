#ifndef _8CC2E274_976F_11E2_8445_206A8A22A96A
#define _8CC2E274_976F_11E2_8445_206A8A22A96A

#define GLSTREAMER_CORE_ENUMERATE_BASIC_TYPES(ACTION)\
ACTION(char)\
\
ACTION(signed char)\
ACTION(short int)\
ACTION(int)\
ACTION(long int)\
ACTION(long long int)\
\
ACTION(unsigned char)\
ACTION(unsigned short int)\
ACTION(unsigned int)\
ACTION(unsigned long int)\
ACTION(unsigned long long int)\
\
ACTION(wchar_t)\
ACTION(char16_t)\
ACTION(char32_t)\
\
ACTION(float)\
ACTION(double)\
ACTION(long double)

#endif
