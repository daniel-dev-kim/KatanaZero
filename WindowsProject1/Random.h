#pragma once
class Random {
public:
    Random();

    // 정수 타입
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, T>
        get(T min, T max) {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(rng);
    }

    // 실수 타입
    template<typename T>
    std::enable_if_t<std::is_floating_point_v<T>, T>
        get(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(rng);
    }

    // bool 전용
    bool getBool();

    // chrono::duration 타입 지원
    template<typename Rep, typename Period>
    std::chrono::duration<Rep, Period> get(
        std::chrono::duration<Rep, Period> min,
        std::chrono::duration<Rep, Period> max)
    {
        auto val = get<Rep>(min.count(), max.count());
        return std::chrono::duration<Rep, Period>(val);
    }

    // enum 지원 (기본: enum 값은 연속된 정수로 가정)
    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, T>
        get(T min, T max) {
        using UT = std::underlying_type_t<T>;
        return static_cast<T>(get<UT>(static_cast<UT>(min), static_cast<UT>(max)));
    }

private:
    std::mt19937 rng;
};