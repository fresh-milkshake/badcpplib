#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely used components from Windows headers

// Common definitions for BadCppLib
#define BADCPPLIB_VERSION_MAJOR 1
#define BADCPPLIB_VERSION_MINOR 0  
#define BADCPPLIB_VERSION_PATCH 0

// Compiler macros
#ifdef _MSC_VER
    #define BADCPPLIB_FORCE_INLINE __forceinline
    #define BADCPPLIB_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
    #define BADCPPLIB_FORCE_INLINE __attribute__((always_inline)) inline
    #define BADCPPLIB_NO_INLINE __attribute__((noinline))
#else
    #define BADCPPLIB_FORCE_INLINE inline
    #define BADCPPLIB_NO_INLINE
#endif

// Export/import macros (for Windows DLL)
#ifdef _WIN32
    #ifdef BADCPPLIB_EXPORTS
        #define BADCPPLIB_API __declspec(dllexport)
    #else
        #define BADCPPLIB_API __declspec(dllimport)
    #endif
#else
    #define BADCPPLIB_API
#endif

// Debug macros
#ifdef DEBUG
    #define BADCPPLIB_DEBUG_BREAK() __debugbreak()
    #define BADCPPLIB_ASSERT(expr) \
        do { \
            if (!(expr)) { \
                BADCPPLIB_DEBUG_BREAK(); \
            } \
        } while(0)
#else
    #define BADCPPLIB_DEBUG_BREAK()
    #define BADCPPLIB_ASSERT(expr)
#endif

// Utility macros
#define BADCPPLIB_UNUSED(x) ((void)(x))
#define BADCPPLIB_STRINGIFY(x) #x
#define BADCPPLIB_CONCAT(a, b) a##b

// Macro to disable copying
#define BADCPPLIB_NON_COPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

// Macro to disable moving
#define BADCPPLIB_NON_MOVABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

// Combined macro
#define BADCPPLIB_NON_COPYABLE_NON_MOVABLE(ClassName) \
    BADCPPLIB_NON_COPYABLE(ClassName) \
    BADCPPLIB_NON_MOVABLE(ClassName)

// C++ version check macros
#if __cplusplus >= 201703L
    #define BADCPPLIB_CPP17_OR_LATER
#endif

#if __cplusplus >= 202002L
    #define BADCPPLIB_CPP20_OR_LATER
#endif

// Library namespace
#define BADCPPLIB_NAMESPACE_BEGIN namespace badcpplib {
#define BADCPPLIB_NAMESPACE_END }
