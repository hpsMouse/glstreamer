#include "GLThread.h"

using namespace glstreamer_gl;

__thread void* GLThread::binding_ptr;
__thread std::type_info const* GLThread::binding_type;
__thread glstreamer::ThreadBlock *GLThread::current_threadblock;
