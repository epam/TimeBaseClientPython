#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <exception>
#include <mutex>

namespace DxApiImpl {

std::string string_format(const std::string format, ...);

#define THROW(STRING) \
    throw std::runtime_error(STRING);

#define THROW_EXCEPTION(FORMAT, ...) \
    throw std::runtime_error(string_format(FORMAT, __VA_ARGS__));

class MutexHolder {
public:
    MutexHolder(std::mutex *mutex) {
        this->mutex_ = mutex;
        this->mutex_->lock();
    }

    ~MutexHolder() {
        this->mutex_->unlock();
    }

private:
    std::mutex *mutex_;
};

} // DxApiImpl

#endif //_COMMON_H_