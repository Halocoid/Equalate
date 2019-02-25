#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <chrono>
#include <iostream>

class Log {
public:
    enum class Level : char {
        Trace, Warning, Error
    } CurrentLevel, CutoffLevel;

    Log();

    template<class... T>
    void Write(const T&... a) const;

    template<class... T>
    void Write(const Level cutoff, const T&... a);

private:
    std::string FancyLevels[3];

    template<class T>
    void Print(const T& a) const;

    template<class T, class... TT>
    void Print(const T& a, const TT&... aa) const;
};

#include "Log.tpp"

#endif
