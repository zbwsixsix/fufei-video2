#include "condmutex.h"
#include <stdexcept>
#include <iostream>

CondMutex::CondMutex() {
    // 创建互斥锁
    _mutex = SDL_CreateMutex();
    if (_mutex == nullptr) {
        // 处理创建互斥锁失败的情况
        std::cerr << "Failed to create mutex!" << std::endl;
        throw std::runtime_error("Failed to create mutex!");
    }
    // 创建条件变量
    _cond = SDL_CreateCond();
    if (_cond == nullptr) {
        // 处理创建条件变量失败的情况
        SDL_DestroyMutex(_mutex);  // 先销毁已经创建的互斥锁
        std::cerr << "Failed to create condition variable!" << std::endl;
        throw std::runtime_error("Failed to create condition variable!");
    }
}

CondMutex::~CondMutex() {
    if (_mutex != nullptr) {
        SDL_DestroyMutex(_mutex);
    }
    if (_cond != nullptr) {
        SDL_DestroyCond(_cond);
    }
}

void CondMutex::lock() {
    if (_mutex != nullptr) {
        SDL_LockMutex(_mutex);
    }
}

void CondMutex::unlock() {
    if (_mutex != nullptr) {
        SDL_UnlockMutex(_mutex);
    }
}

void CondMutex::signal() {
    if (_cond != nullptr) {
        SDL_CondSignal(_cond);
    }
}

void CondMutex::broadcast() {
    if (_cond != nullptr) {
        SDL_CondBroadcast(_cond);
    }
}

void CondMutex::wait() {
    if (_cond != nullptr && _mutex != nullptr) {
        SDL_CondWait(_cond, _mutex);
    }
}
