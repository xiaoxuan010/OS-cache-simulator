import subprocess
import json
import pandas as pd
import os
from matplotlib import pyplot as plt
import numpy as np
from scipy.interpolate import UnivariateSpline
import seaborn as sns

# 确保输出目录存在
os.makedirs("outputs", exist_ok=True)

# 配置参数
ws_period = 50000
cache_size = 64 * 1024
access_pattern = "localized"
associativity = 4
access_step = 10000
access_counts = range(access_step, 500000 + access_step, access_step)

executable_path = "./build/cache_sim"
working_dir = ".."

results = []

print(f"正在运行模拟（ {len(access_counts)} 数据点）")
print(f"当前目录: {os.path.abspath(working_dir)}")

for i, num_acc in enumerate(access_counts):
    # print(f"访问次数: num_accesses={num_acc}")
        
    # LRU 模拟
    cmd_lru = [
        executable_path,
        "-n", str(num_acc),
        "-s", str(cache_size),
        "-t", access_pattern,
        "-a", str(associativity),
        "-w", str(ws_period),
        "-p", "lru",
        "-j"
    ]
    
    try:
        result_lru = subprocess.run(cmd_lru, capture_output=True, text=True, check=True, cwd=working_dir)
        data_lru = json.loads(result_lru.stdout)
        lru_hit_rate = data_lru['average']['hit_rate']

        # LFU 模拟
        cmd_lfu = [
            executable_path,
            "-n", str(num_acc),
            "-s", str(cache_size),
            "-t", access_pattern,
            "-a", str(associativity),
            "-w", str(ws_period),
            "-p", "lfu",
            "-j"
        ]
        
        result_lfu = subprocess.run(cmd_lfu, capture_output=True, text=True, check=True, cwd=working_dir)
        data_lfu = json.loads(result_lfu.stdout)
        lfu_hit_rate = data_lfu['average']['hit_rate']

        results.append({
            "num_accesses": num_acc,
            "lru_hit_rate": lru_hit_rate,
            "lfu_hit_rate": lfu_hit_rate,
        })
    except subprocess.CalledProcessError as e:
        print(f"运行模拟出错 num_accesses={num_acc}: {e}")

df = pd.DataFrame(results)
output_csv_path = f"outputs/cache_hit_rate_vs_num_accesses_w{ws_period}_s{cache_size}_p{access_pattern}_a{associativity}.csv"
df.to_csv(output_csv_path, index=False)
print(f"数据已保存至: {output_csv_path}")

# 绘图
plt.rcParams["font.sans-serif"] = ["LXGW ZhenKai GB", "SimHei", "DejaVu Sans"]

fig, ax1 = plt.subplots(1, 1, figsize=(8, 6))

# 原始数据点
ax1.plot(
    df["num_accesses"],
    df["lru_hit_rate"],
    marker="o",
    markevery=2,
    label=None,
    linewidth=0,
    markersize=4,
    color=sns.color_palette("pastel")[0],
)
ax1.plot(
    df["num_accesses"],
    df["lfu_hit_rate"],
    marker="o",
    markevery=2,
    label=None,
    linewidth=0,
    markersize=4,
    color=sns.color_palette("pastel")[1],
)

# 样条曲线拟合
try:
    spl_lru = UnivariateSpline(df["num_accesses"], df["lru_hit_rate"], s=10)

    # LFU 分段拟合 (在 ws_period 处分段)
    split_point = ws_period
    df_lfu_1 = df[df["num_accesses"] <= split_point]
    df_lfu_2 = df[df["num_accesses"] >= split_point]

    # 第一段 (<= 50000): 使用插值 (s=0) 确保精确通过峰值点
    if len(df_lfu_1) > 3:
        spl_lfu_1 = UnivariateSpline(df_lfu_1["num_accesses"], df_lfu_1["lfu_hit_rate"], s=0)
        x_smooth_1 = np.linspace(df_lfu_1["num_accesses"].min(), df_lfu_1["num_accesses"].max(), 100)
        y_lfu_smooth_1 = spl_lfu_1(x_smooth_1)
    else:
        x_smooth_1 = df_lfu_1["num_accesses"]
        y_lfu_smooth_1 = df_lfu_1["lfu_hit_rate"]

    # 第二段 (>= 50000): 使用加权平滑，强制起始点连接
    if len(df_lfu_2) > 3:
        w2 = np.ones(len(df_lfu_2))
        w2[0] = 1e6  # 给连接点极高权重
        spl_lfu_2 = UnivariateSpline(df_lfu_2["num_accesses"], df_lfu_2["lfu_hit_rate"], w=w2, s=10)
        x_smooth_2 = np.linspace(df_lfu_2["num_accesses"].min(), df_lfu_2["num_accesses"].max(), 200)
        y_lfu_smooth_2 = spl_lfu_2(x_smooth_2)
    else:
        x_smooth_2 = df_lfu_2["num_accesses"]
        y_lfu_smooth_2 = df_lfu_2["lfu_hit_rate"]

    x_lfu_smooth = np.concatenate([x_smooth_1, x_smooth_2])
    y_lfu_smooth = np.concatenate([y_lfu_smooth_1, y_lfu_smooth_2])

    # 生成平滑曲线 (LRU)
    x_smooth = np.linspace(df["num_accesses"].min(), df["num_accesses"].max(), 300)
    y_lru_smooth = spl_lru(x_smooth)

    # 绘制平滑曲线
    ax1.plot(
        x_smooth,
        y_lru_smooth,
        linewidth=2,
        label="LRU",
        color=sns.color_palette("pastel")[0],
    )
    ax1.plot(
        x_lfu_smooth,
        y_lfu_smooth,
        linewidth=2,
        label="LFU",
        color=sns.color_palette("pastel")[1],
    )
except Exception as e:
    print(f"曲线拟合失败: {e}")

ax1.set_xlabel("访问次数 Number of Accesses")
ax1.set_ylabel("命中率 Hit Rate (%)")
ax1.set_title("LRU vs LFU Hit Rate Comparison (vs Accesses)")
ax1.legend()
ax1.grid(True, alpha=0.3)

# 添加测试参数说明
param_text = (
    f"工作集周期: {ws_period}\n"
    f"缓存大小: {cache_size // 1024} KB\n"
    f"相联度: {associativity} 路组相联\n"
    f"访问模式: {access_pattern}"
)
props = dict(boxstyle='round', facecolor='white', alpha=0.5)
ax1.text(0.97, 0.05, param_text, transform=ax1.transAxes, fontsize=12,
        verticalalignment='bottom', horizontalalignment='right', 
        multialignment='left', bbox=props)

# 标出 LFU 的最高值
if not df.empty:
    max_lfu_row = df.loc[df["lfu_hit_rate"].idxmax()]
    max_lfu_x = int(max_lfu_row["num_accesses"])
    max_lfu_y = max_lfu_row["lfu_hit_rate"]

    ax1.axvline(
        x=max_lfu_x,
        color=sns.color_palette("pastel")[1],
        linestyle="--",
        alpha=0.7,
        linewidth=1.5,
    )
    ax1.text(
        max_lfu_x,
        max_lfu_y + 1,
        f"n={max_lfu_x}",
        ha="center",
        va="bottom",
        fontsize=10,
        color=sns.color_palette("muted")[1],
    )

plt.tight_layout()
output_img_path = f"outputs/cache_hit_rate_comparison_vs_accesses_w{ws_period}_s{cache_size}_p{access_pattern}_a{associativity}.png"
plt.savefig(output_img_path, dpi=300, bbox_inches="tight")
print(f"图表已保存至: {output_img_path}")
plt.show()
