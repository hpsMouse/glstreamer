#ifndef _998BBFA6_A987_11E2_A020_206A8A22A96A
#define _998BBFA6_A987_11E2_A020_206A8A22A96A

#include <GL/glew.h>

inline GLEWContext* glewGetContext()
{
    static __thread GLEWContext context;
    return &context;
}

#endif
