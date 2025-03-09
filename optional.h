#pragma once
#include <stdexcept>
#include <utility>
#include <optional>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};


//первое что отличается от реализации в инете это то что у меня optional хранит не само значение а указатель.!!!!
template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value)
    {
        //if (value == std::nullopt) return;
        T* obj = new (&data_[0]) T(value);
        obj_ = obj;
        is_initialized_ = true;
    }
    Optional(T&& value) noexcept
    {
        //if (value == std::nullopt) return;
        T* obj = new (&data_[0]) T(std::move(value));
        std::swap(obj, obj_);
        is_initialized_ = true;
    }

    Optional(const Optional& other)
    {
        if (other.HasValue())
        {
            obj_ = new (&data_[0]) T(other.Value());
            is_initialized_ = true;
        }
    }

    Optional(Optional&& other) noexcept
    {
        if (other.HasValue())
        {
            obj_ = new (&data_[0]) T(std::move(other.Value()));
            is_initialized_ = true;
        }
    }
    
    Optional& operator=(const T& value)
    {
        if (HasValue()) Reset();
        obj_ = new (&data_[0]) T(value);
        is_initialized_ = true;
        return *this;
    }
    Optional& operator=(T&& rhs){
        if (HasValue()) Reset();
        obj_ = new (&data_[0]) T(std::move(rhs));
        is_initialized_ = true;
        return *this;
    }
    Optional& operator=(const Optional& rhs) {
        if (this == &rhs) return *this;

        if (is_initialized_ && rhs.HasValue())
        {
            //Reset();
            *obj_ = *rhs.obj_;
        }
        else {
            //Reset();
            try {
                obj_ = new (&data_[0]) T(rhs.Value());
            }
            catch (const BadOptionalAccess& ex) {
                Reset();
                return *this;
            }
            is_initialized_ = true;
        }
        return *this;
    }
    Optional& operator=(Optional&& rhs) noexcept {
        if (this == &rhs) return *this;

        if (is_initialized_ && rhs.HasValue())
        {
            *obj_ = std::move(rhs.Value());
        }
        else {
            try {
                obj_ = new (&data_[0]) T(std::move(rhs.Value()));
            }
            catch (const BadOptionalAccess& ex)
            {
                Reset();
                return *this;
            }
            is_initialized_ = true;
        }
        return *this;
    }
    
    ~Optional()
    {
        Destroy();
    }
    
   
    

    template <typename ... Args>
    void Emplace(Args&& ... values)
    {
        if (is_initialized_) Destroy();

        T* obj = new T(std::forward<Args>(values)...);
        obj_ = obj;
        is_initialized_ = true;
    }

    bool HasValue() const { return is_initialized_; }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    
    
    
    T& operator*() &
    {
        return static_cast<T&>(*obj_);
    }
    const T& operator*() const&
    {
        return static_cast<const T&>(*obj_);
    }

    T&& operator*() &&
    {
        return std::move(*obj_);
    }

    T* operator->() &
    {
        return static_cast<T*>(obj_);
    }
    const T* operator->() const&
    {
        return static_cast<const T*>(obj_);
    }
    

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() &
    {
        if (!is_initialized_) throw BadOptionalAccess{};

        return static_cast<T&>(*obj_);
    }
    const T& Value() const&
    {
        if (!is_initialized_) throw BadOptionalAccess{};

        return static_cast<const T&>(*obj_);
    }
    
    T&& Value() && {
        if (!is_initialized_) throw BadOptionalAccess{};

        return std::move(*obj_);
    }
    

    void Reset()
    {
        if (!is_initialized_) return;

        is_initialized_ = false;
        obj_->~T();
    }

private:

    void Destroy()
    {
        if (is_initialized_) {
            obj_->~T();
            is_initialized_ = false;
        }
    }
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    T* obj_;
    bool is_initialized_ = false;
};