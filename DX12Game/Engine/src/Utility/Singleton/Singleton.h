#pragma once

// 基底クラス
template <typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        static T& s_instance = T::createInstance();
        return s_instance;
    }

protected:
    Singleton() {}
    virtual ~Singleton() {}

private:
    // コピー・ムーブ禁止
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

// シングルトン化のマクロ
#define SINGLETON(Type) \
    friend class Singleton<Type>; \
    Type() = default; \
    ~Type() = default; \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete; \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;