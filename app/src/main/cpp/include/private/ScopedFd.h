#pragma once

#include <unistd.h>

/**
 * Simple RAII wrapper for file descriptors
 * Similar to Android's ScopedFd but simplified for our use case
 */
class ScopedFd {
public:
    ScopedFd() : fd_(-1) {}
    
    explicit ScopedFd(int fd) : fd_(fd) {}
    
    ~ScopedFd() {
        reset();
    }
    
    // Move constructor
    ScopedFd(ScopedFd&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }
    
    // Move assignment
    ScopedFd& operator=(ScopedFd&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }
    
    // Delete copy constructor and assignment
    ScopedFd(const ScopedFd&) = delete;
    ScopedFd& operator=(const ScopedFd&) = delete;
    
    int get() const { return fd_; }
    
    void reset(int fd = -1) {
        if (fd_ != -1) {
            close(fd_);
        }
        fd_ = fd;
    }
    
    int release() {
        int fd = fd_;
        fd_ = -1;
        return fd;
    }
    
    bool isValid() const { return fd_ != -1; }
    
private:
    int fd_;
};
