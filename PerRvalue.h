#pragma once
#include <exception>
#include <iostream>
#include <sstream>

using namespace std;

class Gold {
public:
    static Gold Mine(int amount) {
        if (amount <= 0) {
            throw std::out_of_range("Invalid amount of gold.");
        }
        cout << "Mine " << amount << " units of gold. ";
        if (amount > available_) {
            // Недостаточно золота в недрах планеты
            ostringstream msg;
            msg << "Out of gold. Only " << available_ << " units are available.";
            throw runtime_error(msg.str());
        }
        available_ -= amount;
        cout << available_ << " units left." << endl;
        return Gold(amount);
    }

    static int GetAvailableAmount() {
        return available_;
    }

    [[nodiscard]] Gold Take(int amount) {
        if (amount > amount_ || amount < 0) {
            throw std::invalid_argument("Invalid gold amount"s);
        }
        amount_ -= amount;
        return Gold(amount);
    }

    int GetAmount() const noexcept {
        return amount_;
    }


    Gold(const Gold& other) {
        *this = Mine(other.amount_);
    }
    // Просто перемещаем золото из одного места в другое
    Gold(Gold&& other) noexcept
        : amount_(other.amount_)  //
    {
        other.amount_ = 0;
    }

    Gold& operator=(const Gold& rhs) {
        if (this != &rhs) {
            Dispose();  // Возвращаем прежнее количество золота
            *this = Mine(rhs.amount_); // И добываем новое
        }
        return *this;
    }
    Gold& operator=(Gold&& rhs) noexcept {
        Dispose(); // Возвращаем прежнее количества золота
        // ... и забираем содержимое rhs
        amount_ = rhs.amount_;
        rhs.amount_ = 0;
        return *this;
    }

    ~Gold() {
        Dispose();
    }
    
private:
    // Конструктор объявлен приватным, чтобы создание золота было возможным только путём его добычи
    explicit Gold(int amount) noexcept
        : amount_(amount) {
    }

    void Dispose() noexcept {
        if (amount_ != 0) {
            available_ += amount_;
            cout << "Dispose "sv << amount_ << " units of gold. "sv << available_
                << " units available."sv << endl;
            amount_ = 0;
        }
    }

    int amount_{};

    // Количество золота в недрах Земли
    static inline int available_ = 100;
};

class Person {
public:
    Person(string name, Gold gold)
        : name_(move(name))
        , gold_(move(gold))  //
    {
        cout << "Person "sv << name_ << " was created with "sv << gold_.GetAmount()
            << " units of gold."sv << endl;
    }

    // Запрещаем клонирование людей
    Person(const Person& other) = delete;
    Person& operator=(const Person&) = delete;

    ~Person() {
        cout << "Person "sv << name_ << " died."sv << endl;
    }

    [[nodiscard]] const Gold& GetGold() const& {
        return gold_;
    }
    // Метод вызывается у объектов, доступных по rvalue-ссылке
    [[nodiscard]] Gold GetGold() && = delete;
    /*
    {
        return std::move(gold_);
    }
    */

private:
    string name_;
    Gold gold_;
};

Person MakePersonWithGold() {
    return Person("Scrooge McDuck"s, Gold::Mine(60));
}

void TestRvalue() {
    try {
        Person SM =  MakePersonWithGold();
        Gold gold = SM.GetGold();
        Person chip("Chip"s, gold.Take(20));
        Person dale("Dale"s, gold.Take(20));
        Person donald("Donald Duck"s, gold.Take(20));
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }

    cout << Gold::GetAvailableAmount() << " units of gold left."s << endl;
}