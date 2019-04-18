#include "stdafx.h"
#include "scopedguard.h"

ScopedGuard::ScopedGuard(std::function<void(void)> f)
    : m_f(f)
{
    
}

ScopedGuard::~ScopedGuard()
{
    m_f();
}
