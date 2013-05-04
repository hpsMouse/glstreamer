#ifndef _E4E6C056_B321_11E2_AC3C_206A8A22A96A
#define _E4E6C056_B321_11E2_AC3C_206A8A22A96A

#include <iostream>
#include <stdexcept>
#include <thread>
#include <typeinfo>

#include "gl.inc.h"

class B;
namespace glstreamer_gl
{
    class GLThread
    {
    private:
        // Just a workaround for GCC bug 41933
        // GCC seems too buggy on C++11, Maybe I should try clang for my next
        // project
        template <typename Binding>
        struct ThreadFunc;
        template <typename Binding>
        friend struct ThreadFunc;
        template <typename Binding>
        struct ThreadFunc
        {
            // NOTE: Args is for Binding, NOT for Func!!
            template <typename Func, typename ... Args>
            void operator() (Func&& func, Args&& ... args)
            {
                try
                {
                    Binding binding(args...);
                    GLThread::binding_ptr = &binding;
                    GLThread::binding_type = &typeid(Binding);
                    if(glewInit() != GLEW_OK)
                        throw std::runtime_error("GLEW init error.");
                    func();
                }
                catch(std::exception const& e)
                {
                    std::cerr << "Thread terminated by an exception: " << e.what() << std::endl;
                }
            }
        };
        
    public:
        /**
         * \brief Start a new thread in an OpenGL context.
         * 
         * The second argument exists for template argument deduction only. Its
         * value is never used.
         */
        template <typename Func, typename Binding, typename ... BindingArgs>
        explicit GLThread(Func&& f, Binding const*, BindingArgs&& ... args):
        realThread(ThreadFunc<Binding>(), f, args...)
        {}
        
        void join()
        {
            realThread.join();
        }
        
        template <typename Binding>
        static Binding* getGLContextBinding()
        {
            if(*binding_type == typeid(Binding))
                return static_cast<Binding*>(binding_ptr);
            else
                return nullptr;
        }
        
    private:
        
        static __thread void* binding_ptr;
        static __thread std::type_info const* binding_type;
        
        std::thread realThread;
    };
}

#endif
