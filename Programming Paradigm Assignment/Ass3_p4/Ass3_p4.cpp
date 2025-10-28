//2458620 古贺 终于后悔反省了

//Edition 1
// 正确性**：数学上正确（递归定义直接实现）。  
//问题：指数级时间（重叠子问题大量重复计算），很快耗尽时间 / 栈且结果易溢出（int）。
//建议修复：用于学习 / 演示小 n（比如 n <= 10）；若用于更大 n，应添加记忆化或改用 DP。
//
//特点 / 用例：
//
//优点：实现直观、短小、便于理解卡特兰递推定义。
//
//缺点：性能糟糕（O(C_n) 递归调用数），不适合实用情形。
//
//适用场景：教学、证明或极小 n 的快速实验。
//
//新知识：无（这是标准递归）。
// Returns the n-th Catalan number using a plain recursive definition.
//
// This function directly implements C_n = sum_{i=0..n-1} C_i * C_{n-1-i}.
// Params:
//   n: non-negative index of Catalan sequence.
// Returns:
//   The n-th Catalan number as int (may overflow for modest n).
int catalan_recursive(int n) {
    if (n <= 1) return 1;
    int res = 0;
    for (int i = 0; i < n; ++i) {
        res += catalan_recursive(i) * catalan_recursive(n - 1 - i);
    }
    return res;
}

//Edition 2
// **正确性**：正确（动态规划自底向上）。  
//问题：若 int 可能溢出；需包含 <vector>（上面补齐）。复杂度 O(n²)。
//
//特点 / 用例：
//
//优点：时间 O(n²)，空间 O(n)，稳定且实用于中等 n（取决于整型范围）。
//
//适合：常规场景，需要 n 不太大或使用更大整数类型。
//
//新知识：无，标准 DP。
// Compute the n-th Catalan number using iterative dynamic programming.
//
// Params:
//   n: non-negative index.
// Returns:
//   n-th Catalan number as int (may overflow for large n).
#include <vector>

int catalan_iterative(int n) {
    if (n <= 1) return 1;
    std::vector<int> dp(n + 1, 0);
    dp[0] = dp[1] = 1;
    for (int i = 2; i <= n; ++i)
        for (int j = 0; j < i; ++j)
            dp[i] += dp[j] * dp[i - 1 - j];
    return dp[n];
}

//Edition 3
// 正确性**：逻辑正确。  
//问题：递归深度为 n（通常可接受），但实现中对 memo 的递归调用会多次重复调用子问题——不过由于 memo 存储，重复计算会减少到 O(n²) 级别（与 DP 相同复杂度）。long long 仍然会在较小 n 后溢出。
//
//特点 / 用例：
//
//优点：自顶向下、易于实现带缓存的递归，可在只需要部分 n 值（或动态查询多个 n）时有用。
//
//适合：当你想保留计算过程并增量求值（懒计算）或需要递归结构保留时。
//
//值得注意的细节：unordered_map 用作记忆化容器；注意对 memo 的引用传递以避免拷贝。


// Compute the n-th Catalan number using top-down recursion with memoization.
//
// Params:
//   n: non-negative index.
//   memo: reference to a map used as cache; entries will be populated.
// Returns:
//   n-th Catalan number as long long (may overflow for moderate n).

#include <unordered_map>

long long catalan_memoization(int n, std::unordered_map<int, long long>& memo) {
    if (n <= 1) return 1;
    if (memo.find(n) != memo.end()) return memo[n];
    long long res = 0;
    for (int i = 0; i < n; ++i)
        res += catalan_memoization(i, memo) * catalan_memoization(n - 1 - i, memo);
    return memo[n] = res;
}

//Edition 4
// **正确性**：正确且可处理任意大 n（受时间/内存限制）。  
//问题：需要链接 Boost.Multiprecision，时间复杂度 O(n²)；大 n 时性能和内存消耗显著。
//
//特点 / 用例：
//
//优点：不受原生整型溢出限制，可以得到精确大整数结果。
//
//适合：计算较大 n 的卡特兰数并需要精确值（例如组合学研究、数学工具）。
//
//新知识：Boost.Multiprecision 的 cpp_int 用法（对 C++ 标准库用户有用）。
// Compute the n-th Catalan number using dynamic programming with Boost cpp_int.
//
// Requires Boost.Multiprecision.
// Params:
//   n: non-negative index.
// Returns:
//   n-th Catalan number as cpp_int (arbitrary precision).

#include <boost/multiprecision/cpp_int.hpp>
#include <vector>

boost::multiprecision::cpp_int catalan_bigint(int n) {
    using boost::multiprecision::cpp_int;
    if (n <= 1) return 1;
    std::vector<cpp_int> dp(n + 1);
    dp[0] = dp[1] = 1;
    for (int i = 2; i <= n; ++i) {
        dp[i] = 0;
        for (int j = 0; j < i; ++j)
            dp[i] += dp[j] * dp[i - 1 - j];
    }
    return dp[n];
}

//Edition 5
// **正确性**：数学公式正确（(C_n = \frac{(2n)!}{(n+1)! n!})）。  
//问题：直接用 factorial 和 long long 很快溢出（即使中间(2n)!超过 long long），且直接除法可能造成暂时非整除的浮点 / 截断问题（这里用整数，数学上整除成立，但溢出问题最致命）。
//修复：使用 渐进乘除法（逐步乘并除以 i）或使用大整数库或 __int128 并使用约简的方法计算组合数而不是先算(2n)!。
//
//特点 / 用例：
//
//优点：常数时间风格（O(n) 阶乘计算），理论上比 O(n²) DP 更快，但中间阶乘增长大。
//
//适合：当配合大整数或谨慎的乘除约简实现时，可以高效计算较大的 n。
//
//值得注意细节：要避免中间溢出，应用乘 / 除交替或使用高精度整数。


// Compute Catalan number via factorial formula Cn = (2n)! / ((n+1)! * n!).
//
// Params:
//   n: non-negative index.
// Returns:
//   n-th Catalan number as long long (will overflow for modest n).
//
// Note:
//   For large n use arbitrary-precision or multiplicative/binomial algorithm to avoid overflow.
long long factorial(int n) {
    long long res = 1;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

long long catalan_formula(int n) {
    if (n <= 1) return 1;
    return factorial(2 * n) / (factorial(n + 1) * factorial(n));
}
//Edition 6
// **正确性**：逻辑正确 但 constexpr 的实际编译期求值依赖于是否在编译期调用。该函数若在 constexpr 上下文中被求值（而 n 是常量），编译器会在编译期计算。  
//问题：long long dp[64] 假设 n <= 63，否则越界。constexpr 函数里使用可变数组和循环在现代 C++ 是允许的，但编译器对多复杂的 constexpr 计算可能限制。long long 溢出问题依然存在。
//
//特点 / 用例：
//
//优点：若 n 是编译期常量，能把值计算到编译期，避免运行时开销。
//
//适合：嵌入到静态配置或模板中需要已知常量的场景。
//
//值得注意的细节：constexpr 函数可以包含循环 / 数组，只要在编译期求值时满足常量条件；但存在编译器资源限制。
// Compute Catalan number possibly at compile time when `n` is a compile-time constant.
//
// Params:
//   n: non-negative index (must be <= 63 for this implementation's storage).
// Returns:
//   n-th Catalan number as long long (may overflow for moderate n).
//
// Note:
//   This function can be evaluated at compile-time only when called in a constexpr context.
constexpr long long catalan_constexpr(int n) {
    if (n <= 1) return 1;
    long long dp[64] = { 0 }; // 支持 n <= 63
    dp[0] = dp[1] = 1;
    for (int i = 2; i <= n; ++i) {
        dp[i] = 0;
        for (int j = 0; j < i; ++j)
            dp[i] += dp[j] * dp[i - 1 - j];
    }
    return dp[n];
}
//Edition 7
// **正确性**：有问题/低效。主要问题：

//只对一侧（i）并行化，但对 n - 1 - i 侧仍然串行调用 catalan_parallel(n - 1 - i)，导致大量重复计算和不均衡。
//
//每次递归都会创建大量 std::async 任务（线程 / 任务膨胀），会严重超载线程池和造成高开销。
//
//结果仍然会大量重复且溢出风险高。
//
//需要包含 <future> <vector>。
//
//修复建议：使用带记忆化的并行方案（例如并行化顶层若干独立子问题 + 缓存结果，或用任务窃取 / 线程池更细粒度控制），并避免对小 i 使用 async（阈值控制）。
//
//特点 / 用例：
//
//优点：尝试并行化递归以利用多核。
//
//缺点：未作阈值 / 缓存控制会产生极高的任务开销，通常比串行更慢。
//
//适合：如果要并行化，需配合缓存与任务划分策略，仅对较高层次子问题并行化。
//
//值得注意的细节：
//
//std::async 的行为依赖于实现，对于大量短任务开销大。
//
//并行递归需要阈值、缓存和合适的任务粒度管理。
// Attempt to compute Catalan number in parallel using std::async.
//
// WARNING: naive use of std::async per-subtask causes explosion of tasks and is inefficient.
// Params:
//   n: non-negative index.
// Returns:
//   n-th Catalan number as long long (may overflow).
#include <future>
#include <vector>

long long catalan_parallel(int n) {
    if (n <= 1) return 1;
    std::vector<std::future<long long>> futures;
    long long res = 0;
    for (int i = 0; i < n; ++i) {
        futures.push_back(std::async(std::launch::async, catalan_parallel, i));
    }
    for (int i = 0; i < n; ++i) {
        res += futures[i].get() * catalan_parallel(n - 1 - i);
    }
    return res;
}
//Edition 8
// **正确性**：逻辑正确（在 n>=0 的前提下返回 optional）。  
//问题：std::optional 只是包装返回值以指示无效 n，但并没有处理溢出 / 错误检测。若想更“安全”，应检测溢出并返回 std::nullopt 或更大类型。
//
//特点 / 用例：
//
//优点：接口上表明 n 的有效性，适合 API 需要显式错误信号的场景。
//
//建议：可与溢出检测配合（检查乘法溢出）以提供真正确保。
//
//值得注意的细节：std::optional 用于标示“无值”状态；需要包含 <optional>。


// Compute n-th Catalan number with boundary check using std::optional for error signalling.
//
// Params:
//   n: index (non-negative expected).
// Returns:
//   optional with value on success, std::nullopt on invalid input. Does NOT detect overflow.
#include <optional>
#include <vector>

std::optional<long long> catalan_safe(int n) {
    if (n < 0) return std::nullopt;
    if (n <= 1) return 1;
    std::vector<long long> dp(n + 1);
    dp[0] = dp[1] = 1;
    for (int i = 2; i <= n; ++i)
        for (int j = 0; j < i; ++j)
            dp[i] += dp[j] * dp[i - 1 - j];
    return dp[n];
}
//Edition 9
// 正确性**：修正后在编译期可计算（若 N 小且 unsigned long long 不溢出）。原始版本语法错误。

//特点 / 用例：
//
//优点：当 N 是编译期常量且较小，可在编译期计算并将值作为常量嵌入代码。
//
//缺点：编译时间可能长，且模板递归与常数表达限制（以及溢出）。
//
//值得注意的细节：C++14 / 17 可用编译期 lambda 或 constexpr 静态初始化技巧。模板元编程可以把常数“泛化”到类型层面。
// Compile-time computation of Catalan numbers via template metaprogramming.
//
// Template parameter N must be a compile-time constant and small enough to avoid overflow.
// Usage: Catalan<5>::value
template <unsigned N>
struct Catalan {
  static constexpr unsigned long long value = []() constexpr {
    unsigned long long sum = 0;
    for (unsigned i = 0; i < N; ++i)
      sum += Catalan<i>::value * Catalan<N - 1 - i>::value;
    return sum;
  }();
};

template <>
struct Catalan<0> { static constexpr unsigned long long value = 1ULL; };
template <>
struct Catalan<1> { static constexpr unsigned long long value = 1ULL; };

//Edition 10
// 正确性**：误导/不相关。该 base 矩阵是 Fibonacci 的 2x2 矩阵（Fib 的快速幂），与卡特兰数并没有简单的 2×2 矩阵线性递推关系（卡特兰数满足线性递推吗？不 — 它是一个非线性卷积递推）。因此该实现不是卡特兰数，只是示例矩阵乘法。  
//结论：版本 10 无效作为卡特兰数实现，需要完整不同的线性化方案（若存在）或说明这是概念演示。
//
//特点 / 用例：
//
//优点：展示矩阵乘法 / 快速幂技巧（对 Fibonacci 有效）。
//
//但不适合计算卡特兰数。
//
//值得注意细节：误用 Fibonacci 矩阵会误导读者；卡特兰数的递推不是线性的二阶常系数递推，不能用简单 2×2 矩阵幂解决。
// Demonstration of 2x2 matrix multiplication (Fibonacci-style).
// NOTE: This function does NOT compute Catalan numbers; it demonstrates matrix power concept.
#include <array>

using Matrix2x2 = std::array<std::array<long long, 2>, 2>;
Matrix2x2 matrix_multiply(const Matrix2x2& a, const Matrix2x2& b) {
    Matrix2x2 result = { {{0, 0}, {0, 0}} };  
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

long long catalan_matrix(int n) {
    if (n <= 1) {
        return 1;  
    }

    Matrix2x2 base = { {{1, 1}, {1, 0}} };
    Matrix2x2 result = { {{1, 0}, {0, 1}} };
    for (int i = 0; i < n; ++i) {
        result = matrix_multiply(result, base);
    }

    return result[0][0];
}

//Edition MyVersion
#include <boost/multiprecision/cpp_int.hpp>

/**
 * Compute the n-th Catalan number with arbitrary precision using Boost.Multiprecision.
 * Uses multiplicative approach to compute binomial coefficient:
 *   C(2n,n) = product_{i=1..n} (n + i) / i
 * then divides by (n+1).
 *
 * Params:
 *   n: non-negative index.
 * Returns:
 *   n-th Catalan number as boost::multiprecision::cpp_int.
 *
 * Complexity:
 *   Time: O(n) big-int operations, Space: O(1) extra aside from return.
 */
boost::multiprecision::cpp_int CatalanBig(int n) {
    using boost::multiprecision::cpp_int;
    if (n <= 1) return cpp_int(1);
    cpp_int res = 1;
    for (int i = 1; i <= n; ++i) {
        res *= cpp_int(n + i);
        res /= cpp_int(i);
    }
    res /= cpp_int(n + 1);
    return res;
}
