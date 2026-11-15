static double seconds(){
#ifdef _WIN32
    static const double frequency = []{
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return static_cast<double>(value.QuadPart);
    }();
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return static_cast<double>(value.QuadPart) / frequency;
#else
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
}
