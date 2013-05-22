#ifndef _E4E6C056_B321_11E2_AC3C_206A8A22A96A
#define _E4E6C056_B321_11E2_AC3C_206A8A22A96A

#include <iostream>
#include <stdexcept>
#include <thread>
#include <typeinfo>

#include <posixpp/PThreadBarrier.h>

#include "gl.inc.h"

#include "../ThreadBlock.h"

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
            ThreadFunc(GLThread *thread):
            thread(thread)
            {}
            
            GLThread *thread;
            
            // NOTE: Args is for Binding, NOT for Func!!
            template <typename Func, typename ... Args>
            void operator() (Func&& func, Args&& ... args)
            {
                try
                {
                    Binding binding(args...);
                    initGLContextBinding(binding);
                    
                    glstreamer::ThreadBlock threadBlock;
                    current_threadblock = &threadBlock;
                    thread->threadBlock_ = &threadBlock;
                    
                    thread->initBarrier->wait();
                    
                    func();
                }
                catch(std::exception const& e)
                {
                    std::cerr << "Thread terminated by an exception: " << e.what() << std::endl;
                }
            }
        };
        
    public:
        GLThread(): threadBlock_(nullptr), initBarrier(), realThread() {}
        
        /**
         * \brief Start a new thread in an OpenGL context.
         * 
         * The second argument exists for template argument deduction only. Its
         * value is never used.
         */
        template <typename Func, typename Binding, typename ... BindingArgs>
        explicit GLThread(Func&& f, Binding const*, BindingArgs&& ... args):
        threadBlock_(),
        initBarrier(new posixpp::PThreadBarrier(2)),
        realThread(ThreadFunc<Binding>(this), f, args...)
        {
            initBarrier->wait();
            initBarrier.reset();
        }
        
        GLThread(GLThread&&) = default;
        GLThread& operator = (GLThread&&) = default;
        
        void join()
        {
            realThread.join();
        }
        
        glstreamer::ThreadBlock& threadBlock()
        {
            return *threadBlock_;
        }
        
        static glstreamer::ThreadBlock& currentThreadBlock()
        {
            return *current_threadblock;
        }
        
        template <typename Binding>
        static Binding* getGLContextBinding()
        {
            if(*binding_type == typeid(Binding))
                return static_cast<Binding*>(binding_ptr);
            else
                return nullptr;
        }
        
        template <typename Binding>
        static void initGLContextBinding(Binding &binding)
        {
            GLThread::binding_ptr = &binding;
            GLThread::binding_type = &typeid(Binding);
            if(glewInit() != GLEW_OK)
                throw std::runtime_error("GLEW init error.");
        }
        
    private:
        GLThread(GLThread const&) = delete;
        GLThread& operator = (GLThread const&) = delete;
        
        static __thread void* binding_ptr;
        static __thread std::type_info const* binding_type;
        static __thread glstreamer::ThreadBlock *current_threadblock;
        
        glstreamer::ThreadBlock *threadBlock_;
        std::unique_ptr<posixpp::PThreadBarrier> initBarrier;
        std::thread realThread;
    };
}

#endif
