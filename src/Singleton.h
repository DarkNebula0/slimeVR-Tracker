#pragma once

template<typename T>
class CSingleton {
protected:
    CSingleton() = default;

    CSingleton(const CSingleton &) = delete;

    CSingleton &operator=(const CSingleton &) = delete;

    virtual ~CSingleton() = default;

public:
    static T *instance() {
        static T s_oInstance;
        return &s_oInstance;
    }
};