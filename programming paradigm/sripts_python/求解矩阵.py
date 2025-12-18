import numpy as np
import matplotlib.pyplot as plt

# ---------------------- 修复中文字体显示 ----------------------
plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']  # Windows优先用黑体，其他系统用默认兼容字体
plt.rcParams['axes.unicode_minus'] = False  # 解决负号显示为方块的问题
# -------------------------------------------------------------

# 定义矩阵A
A = np.array([[12, 6, -6],
              [6, 16, 2],
              [-6, 2, 16]], dtype=np.float64)

# 初始非零向量
v0 = np.array([1.0, 1.0, 1.0])
epsilon = 0.0001

# 按模最大分量函数（返回原始元素）
def max_component(vec):
    idx = np.argmax(np.abs(vec))
    return vec[idx]

# 幂法迭代
lambda_list = []  # 存储各次迭代的特征值近似值
error_list = []   # 存储各次迭代的误差

# 初始步骤
lambda_k = max_component(v0)
u_k = v0 / lambda_k
lambda_list.append(lambda_k)

k = 1
while True:
    # 计算v^(k) = A * u^(k-1)
    v_k = A @ u_k
    # 计算新的特征值近似
    lambda_new = max_component(v_k)
    # 计算误差
    error = np.abs(lambda_new - lambda_k)
    # 记录
    lambda_list.append(lambda_new)
    error_list.append(error)
    # 判断是否收敛
    if error < epsilon:
        break
    # 更新u
    u_k = v_k / lambda_new
    lambda_k = lambda_new
    k += 1

# 输出结果
print(f"按模最大特征值：{lambda_list[-1]:.6f}")
print(f"迭代次数：{k}次")

# 绘制误差曲线
plt.figure(figsize=(8, 4))
plt.plot(range(1, len(error_list)+1), error_list, marker='o', color='b', linestyle='-')
plt.xlabel("迭代轮次")  # 中文标签
plt.ylabel("误差 $|\\lambda_{k+1} - \\lambda_k|$")  # 混合公式和中文
plt.title("幂法迭代的误差变化")  # 中文标题
plt.grid(alpha=0.3)
plt.show()