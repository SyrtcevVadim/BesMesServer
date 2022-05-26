#ifndef SINGLETONBASE_H
#define SINGLETONBASE_H


template <typename T>
class SingletonBase
{
public:
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase &&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;

    static T& getInstance()
    {
        static T instance;
        return instance;
    }

protected:
    SingletonBase() {}
};

#endif // SINGLETONBASE_H
