#include "Alloctor.h"
#include "MemoryMgr.h"


    void*	operator new(size_t nSize)
    {
        return  MemoryMgr::instance().alloc(nSize);
    }

    void	operator delete(void* pMem)
    {
        MemoryMgr::instance().free(pMem);
    }

    void*	operator new[](size_t nSize)
    {
        return  MemoryMgr::instance().alloc(nSize);
    }

    void	operator delete[](void* pMem)
    {
        MemoryMgr::instance().free(pMem);
    }

    void*   mem_alloc(size_t nSize)
    {
        return  MemoryMgr::instance().alloc(nSize);
    }

    void    mem_free(void* pMem)
    {
        MemoryMgr::instance().free(pMem);
    }
