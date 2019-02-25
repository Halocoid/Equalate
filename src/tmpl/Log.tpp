#ifndef LOG_TPP_
#define LOG_TPP_

template<class T, class... TT>
void Log::Print(const T& a, const TT&... aa) const{
    Print(a);
    Print(aa...);
}

template<class T>
void Log::Print(const T& a) const{
    std::cout << a;
}

template<class... T>
void Log::Write(const T&... out) const{
    if (CurrentLevel < CutoffLevel) {
        return;
    }

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    char time[27];
    std::strftime(time, 26, "%a %d %b %Y %H:%M:%S", std::localtime(&now));
    Print(time, FancyLevels[static_cast<int>(CurrentLevel)], out..., "\n");
}

template<class... T>
void Log::Write(const Level cutoff, const T&... out) {
    Level remb = CurrentLevel;
    CurrentLevel = cutoff;
    Write(out...);
    CurrentLevel = remb;
}

#endif
