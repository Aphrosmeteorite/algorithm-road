#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <vector>
#include <iostream>
#include <algorithm>
#include <array>
#include <functional>

namespace Poly
{

    class polynomial;
    // 一项
    class Term
    {
        friend class std::vector<Term>;
        friend class Polynomial;

    public:
        constexpr Term(double c = 0.f, int e = 0) : coef(c), exp(e) {}
        constexpr Term(const Term &t) : coef(t.coef), exp(t.exp) {}
        constexpr Term(Term &&t)
            : Term(0, 0)
        {
            using std::swap;
            swap(coef, t.coef);
            swap(exp, t.exp);
        }
        constexpr Term &operator=(Term other)
        {
            using std::swap;
            swap(coef, other.coef);
            swap(exp, other.exp);
            return *this;
        }
        ~Term() = default;
        // 获取系数和指数
        constexpr inline double getCoef() const noexcept { return coef; }
        constexpr inline double getExp() const noexcept { return exp; }
        // 设置系数和指数
        inline void setCoef(int c) noexcept { coef = c; }
        inline void setExp(int e) noexcept { exp = e; }

        constexpr bool operator==(const Term &t)
            const noexcept
        {
            return exp == t.exp;
        }
        constexpr bool operator!=(const Term &t)
            const noexcept
        {
            return exp != t.exp;
        }
        constexpr bool operator<(const Term &t)
            const noexcept
        {
            return exp < t.exp;
        }
        constexpr bool operator>(const Term &t)
            const noexcept
        {
            return exp > t.exp;
        }
        constexpr bool operator<=(const Term &t)
            const noexcept
        {
            return exp <= t.exp;
        }
        constexpr bool operator>=(const Term &t)
            const noexcept
        {
            return exp >= t.exp;
        }

        // 重载加法运算符
        constexpr Term operator+(const Term &t) const
        {
            if (exp != t.exp)
                throw std::invalid_argument("exponent is not equal");
            if (coef + t.coef == 0)
                return Term(0, 0);
            else
                return Term(coef + t.coef, exp);
        }
        // 重载减法运算符
        constexpr Term operator-(const Term &t) const
        {
            return *this + (Term(-t.coef, t.exp));
        }
        constexpr double cal(double x) const noexcept
        {
            for (int i = 0; i < exp; ++i)
                x *= x;
            return coef * x;
        }

    private:
        double coef; // 系数
        int exp;     // 指数
    };

    class Polynomial
    {
        friend class Term;

    private:
        std::vector<Term> poly; // 多项式
    public:
        void sort(const std::function<bool(const Term &, const Term &)> &cmp = [](const Term &lhs, const Term &rhs)
                  { return lhs.getExp() < rhs.getExp(); }) noexcept
        {
            std::sort(poly.begin(), poly.end(), cmp);
        }

        Polynomial() = default;
        explicit Polynomial(const std::initializer_list<Term> &list)
            : poly(list)
        {
            // 按指数递增排序
            std::sort(poly.begin(), poly.end(),
                      [](const Term &a, const Term &b)
                      { return a.getExp() < b.getExp(); });
        }

        template <std::size_t SIZE>
        explicit Polynomial(const Term (&arr)[SIZE])
        {
            for (std::size_t i = 0; i < SIZE; ++i)
                poly.push_back(arr[i]);
            std::sort(poly.begin(), poly.end(),
                      [](const Term &a, const Term &b)
                      { return a.getExp() < b.getExp(); });
        }

        template <std::size_t SIZE>
        Polynomial(const std::array<Term, SIZE> &arr)
        {
            for (std::size_t i = 0; i < SIZE; ++i)
                poly.push_back(arr[i]);
            std::sort(poly.begin(), poly.end(),
                      [](const Term &a, const Term &b)
                      { return a.getExp() < b.getExp(); });
        }

        Polynomial(const Polynomial &p)
        try : poly(p.poly)
        {
        }
        catch (...)
        {
            std::cerr << "copy constructor failed\n";
            throw;
        }

        constexpr Polynomial(Polynomial &&p)
        try : poly(std::move(p.poly))
        {
        }
        catch (...)
        {
            std::cerr << "move constructor failed\n";
            throw;
        }

        ~Polynomial() = default;

        Polynomial &operator=(Polynomial other) noexcept
        {
            std::swap(poly, other.poly);
            return *this;
        }

        // 重载加法运算符
        Polynomial operator+(const Polynomial &other) const
        {
            Polynomial result;
            auto otherIt = other.poly.begin();
            auto thisIt = poly.begin();
            // 将长度小的作为主循环
            while (thisIt != poly.end() && otherIt != other.poly.end())
            {
                if (thisIt->exp < otherIt->exp)
                {
                    result.poly.push_back(*thisIt);
                    ++thisIt;
                }
                else if (thisIt->exp > otherIt->exp)
                {
                    result.poly.push_back(*otherIt);
                    ++otherIt;
                }
                else
                {
                    if (thisIt->coef + otherIt->coef != 0)
                        result.poly.push_back(*thisIt + *otherIt);
                    ++thisIt;
                    ++otherIt;
                }
            }
            // 将剩余的元素添加到结果中
            while (thisIt != poly.end())
            {
                result.poly.push_back(*thisIt);
                ++thisIt;
            }
            while (otherIt != other.poly.end())
            {
                result.poly.push_back(*otherIt);
                ++otherIt;
            }
            return result;
        }

        // 利用加法重载减法运算符
        // 利用传入参数的拷贝构造函数可以减少代码量
        Polynomial operator-(Polynomial other) const
        {
            for (auto &t : other.poly)
                t.setCoef(-t.getCoef());
            return *this + other;
        }

        void insert(const Term &t)
        {
            for (auto it = poly.begin(); it != poly.end(); ++it)
            {
                if (it->getExp() == t.getExp())
                {
                    it->setCoef(it->getCoef() + t.getCoef());
                    return;
                }
                if (it->getExp() > t.getExp())
                {
                    poly.insert(it, t);
                    return;
                }
            }
        }

        void print(const std::size_t &precision = 2,
                   const std::size_t &width = 6) const
        {
            auto t = poly.begin();
            for (; t != poly.end(); ++t)
            {
                std::cout.precision(precision);
                std::cout.width(width);
                std::cout << t->getCoef() << "x^" << t->getExp() << " ";
            }
            if ((t + 1) == poly.end())
                std::cout << '\n';
        }

        inline bool empty() const noexcept
        {
            return poly.empty();
        }

        inline size_t size() const noexcept
        {
            return poly.size();
        }

        double cal(double x) const noexcept
        {
            // 递归计算多项式的值
            double result = 0;
            for (auto &t : poly)
                result += t.cal(x);
            return result;
        }

        auto begin() noexcept
        {
            return poly.begin();
        }

        auto end() noexcept
        {
            return poly.end();
        }

        auto cbegin() const noexcept
        {
            return poly.cbegin();
        }

        auto cend() const noexcept
        {
            return poly.cend();
        }
    };

    // 编译期可求值的多项式类
    template <std::size_t SIZE>
    class Polynomial_C
    {
    private:
        std::array<Term, SIZE> poly;
        constexpr void sort() noexcept
        {
            auto cmp = [](const Term &lhs, const Term &rhs)
            { return lhs.getExp() < rhs.getExp(); };
            std::sort(poly.begin(), poly.end(), cmp);
        }

    public:
        constexpr Polynomial_C(const std::array<Term, SIZE> &arr) : poly(arr) { sort(); }

        constexpr Polynomial_C(std::array<Term, SIZE> &&arr) : poly(std::move(arr)) { sort(); }

        constexpr Polynomial_C(const std::initializer_list<Term> &list)
            : poly(list) { sort(); }

        constexpr Polynomial_C(const Polynomial_C &p) : poly(p.poly) {}

        constexpr Polynomial_C(Polynomial_C &&p) : poly(std::move(p.poly)) {}
        ~Polynomial_C() = default;

        constexpr const auto cbegin() const noexcept
        {
            return poly.cbegin();
        }

        constexpr const auto cend() const noexcept
        {
            return poly.cend();
        }

        constexpr Polynomial_C &operator=(Polynomial_C other) noexcept
        {
            std::swap(poly, other.poly);
            return *this;
        }

        constexpr Polynomial_C operator+(const Polynomial_C &other) const
        {
            std::array<Term, SIZE> result;
            auto otherIt = other.poly.begin();
            auto thisIt = poly.begin();
            // 将长度小的作为主循环
            while (thisIt != poly.end() && otherIt != other.poly.end())
            {
                if (thisIt->exp < otherIt->exp)
                {
                    result.poly.push_back(*thisIt);
                    ++thisIt;
                }
                else if (thisIt->exp > otherIt->exp)
                {
                    result.poly.push_back(*otherIt);
                    ++otherIt;
                }
                else
                {
                    result.poly.push_back(*thisIt + *otherIt);
                    ++thisIt;
                    ++otherIt;
                }
            }
            // 将剩余的元素添加到结果中
            while (thisIt != poly.end())
            {
                result.poly.push_back(*thisIt);
                ++thisIt;
            }
            while (otherIt != other.poly.end())
            {
                result.poly.push_back(*otherIt);
                ++otherIt;
            }
            return Polynomial_C(result);
        }

        constexpr Polynomial_C operator-(Polynomial_C other) const
        {
            // 利用加法重载减法运算符
            // 利用传入参数的拷贝构造函数可以减少代码量
            for (auto &t : other.poly)
                t.setCoef(-t.getCoef());
            return *this + other;
        }

        void print(const std::size_t &precision = 2,
                   const std::size_t &width = 6) const
        {
            for (auto &t : poly)
            {
                std::cout.precision(precision);
                std::cout.width(width);
                std::cout << t.getCoef() << "x^" << t.getExp() << " ";
            }
            std::cout << '\n';
        }

        constexpr bool empty() const noexcept
        {
            return poly.empty();
        }

        constexpr inline auto &operator[](size_t index) noexcept
        {
            return poly[index];
        }

        constexpr const inline auto &operator[](size_t index) const noexcept
        {
            return poly[index];
        }

        constexpr size_t size() const noexcept
        {
            return poly.size();
        }

        constexpr double cal(double x) const noexcept
        {
            double result = 0;
            for (auto &t : poly)
                result += t.cal(x);
            return result;
        }
    };
} // namespace Poly
#endif // POLYNOMIAL_H