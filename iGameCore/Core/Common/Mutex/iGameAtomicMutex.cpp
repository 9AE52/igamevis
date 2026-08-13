
#include "iGameAtomicMutex.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
// For x86/x86_64 architecture, use Intel's intrinsic function for _mm_pause()
#include <immintrin.h>
#elif defined(__ARM_NEON) || defined(__aarch64__)
// For ARM NEON and ARM 64-bit architectures, define a custom _mm_pause() using inline assembly
static inline void _mm_pause() {
    __asm__ __volatile__("yield" ::: "memory");
    }
#else
// If the platform doesn't support _mm_pause(), define it as an empty function
static inline void _mm_pause() {}
#endif

IGAME_NAMESPACE_BEGIN
iGameAtomicMutex::iGameAtomicMutex()
{
    this->m_Locked = false;
}

// Copy constructor, ensures thread-safe reading of another object's lock state via atomic load
iGameAtomicMutex::iGameAtomicMutex(const iGameAtomicMutex& other) {
    this->m_Locked = other.m_Locked.load(std::memory_order_acquire);
}

// Assignment operator, ensures consistent state during assignment via atomic store
iGameAtomicMutex& iGameAtomicMutex::operator=(const iGameAtomicMutex& other) {
    this->m_Locked.store(other.m_Locked.load(std::memory_order_acquire), std::memory_order_release);
    return *this;
}

// Lock function, implements a spinlock mechanism
void iGameAtomicMutex::lock() {
    while (true) {
        // Attempt atomic exchange, set m_Locked to true, if it was false, lock is acquired
        if (!this->m_Locked.exchange(true, std::memory_order_acquire)) {
            return;
        }
        // If lock acquisition fails, enter a spin-wait loop, using _mm_pause() to reduce CPU busy-wait overhead
        while (this->m_Locked.load(std::memory_order_relaxed)) {
            _mm_pause();
        }
    }
}

// Unlock function, sets the lock state to false to release the lock
void iGameAtomicMutex::unlock() {
    this->m_Locked.store(false, std::memory_order_release);
}
IGAME_NAMESPACE_END