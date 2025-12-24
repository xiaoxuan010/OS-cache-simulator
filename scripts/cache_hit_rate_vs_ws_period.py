import subprocess
import json
import pandas as pd
import os
from matplotlib import pyplot as plt
from matplotlib import font_manager as fm
import numpy as np
from scipy.interpolate import UnivariateSpline
import seaborn as sns

# 确保输出目录存在
os.makedirs("outputs", exist_ok=True)

# 配置参数
num_accesses = 100000
cache_size = 64 * 1024
access_pattern = "localized"
associativity = 4
ws_step = 1000
ws_periods = range(ws_step, 120000, ws_step) 

executable_path = "./build/cache_sim"
working_dir = ".."

results = []

print(f"正在运行模拟（ {len(ws_periods)} 数据点）")
print(f"当前目录: {os.path.abspath(working_dir)}")

for i, ws in enumerate(ws_periods):
    # print(f"工作集周期: ws_period={ws}")
        
    # LRU 模拟
    cmd_lru = [
        executable_path,
        "-n", str(num_accesses),
        "-s", str(cache_size),
        "-t", access_pattern,
        "-a", str(associativity),
        "-w", str(ws),
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
            "-n", str(num_accesses),
            "-s", str(cache_size),
            "-t", access_pattern,
            "-a", str(associativity),
            "-w", str(ws),
            "-p", "lfu",
            "-j"
        ]
        
        result_lfu = subprocess.run(cmd_lfu, capture_output=True, text=True, check=True, cwd=working_dir)
        data_lfu = json.loads(result_lfu.stdout)
        lfu_hit_rate = data_lfu['average']['hit_rate']

        results.append({
            "ws_period": ws,
            "lru_hit_rate": lru_hit_rate,
            # "cmd_lru": cmd_lru,
            "lfu_hit_rate": lfu_hit_rate,
            # "cmd_lfu": cmd_lfu
        })
    except subprocess.CalledProcessError as e:
        print(f"运行模拟出错 ws_period={ws}: {e}")

df = pd.DataFrame(results)
output_csv_path = f"outputs/cache_hit_rate_vs_ws_period_n{num_accesses}_s{cache_size}_p{access_pattern}_a{associativity}.csv"
df.to_csv(output_csv_path, index=False)
print(f"数据已保存至: {output_csv_path}")

# 绘图
plt.rcParams["font.sans-serif"] = ["LXGW ZhenKai GB", "SimHei", "DejaVu Sans"]

fig, ax1 = plt.subplots(1, 1, figsize=(8, 6))

# 原始数据点
ax1.plot(
    df["ws_period"],
    df["lru_hit_rate"],
    marker="o",
    markevery=5,
    label=None,
    linewidth=0,
    markersize=4,
    color=sns.color_palette("pastel")[0],
)
ax1.plot(
    df["ws_period"],
    df["lfu_hit_rate"],
    marker="o",
    markevery=5,
    label=None,
    linewidth=0,
    markersize=4,
    color=sns.color_palette("pastel")[1],
)

# 样条曲线拟合
try:
    spl_lru = UnivariateSpline(df["ws_period"], df["lru_hit_rate"], s=20)
    spl_lfu = UnivariateSpline(df["ws_period"], df["lfu_hit_rate"], s=20)

    # 生成平滑曲线
    x_smooth = np.linspace(df["ws_period"].min(), df["ws_period"].max())
    y_lru_smooth = spl_lru(x_smooth)
    y_lfu_smooth = spl_lfu(x_smooth)

    # 绘制平滑曲线
    ax1.plot(
        x_smooth,
        y_lru_smooth,
        linewidth=2,
        label="LRU",
        color=sns.color_palette("pastel")[0],
    )
    ax1.plot(
        x_smooth,
        y_lfu_smooth,
        linewidth=2,
        label="LFU",
        color=sns.color_palette("pastel")[1],
    )
except Exception as e:
    print(f"曲线拟合失败: {e}")

ax1.set_xlabel("工作集周期 Work Set Period")
ax1.set_ylabel("命中率 Hit Rate (%)")
ax1.set_title("LRU vs LFU Hit Rate Comparison 命中率对比")
ax1.legend()
ax1.grid(True, alpha=0.3)

# 添加测试参数说明
param_text = (
    f"访问次数: {num_accesses}\n"
    f"缓存大小: {cache_size // 1024} KB\n"
    f"相联度: {associativity} 路组相联\n"
    f"访问模式: {access_pattern}"
)
props = dict(boxstyle='round', facecolor='white', alpha=0.5)
ax1.text(0.97, 0.05, param_text, transform=ax1.transAxes, fontsize=12,
        verticalalignment='bottom', horizontalalignment='right', 
        multialignment='left', bbox=props)

# 标出交点的横坐标
if not df.empty:
    df["hit_rate_diff"] = df["lru_hit_rate"] - df["lfu_hit_rate"]
    zero_crossing_idx = (df["hit_rate_diff"] * df["hit_rate_diff"].shift()).lt(0)
    if zero_crossing_idx.any():
        zero_crossing_ws = int(df[zero_crossing_idx]["ws_period"].values[0])
        ax1.axvline(
            x=zero_crossing_ws,
            color=sns.color_palette("pastel")[3],
            linestyle=":",
            alpha=0.7,
            linewidth=1.5,
        )
        ax1.text(
            zero_crossing_ws,
            ax1.get_ylim()[1] * 0.8,
            f"ws={zero_crossing_ws}",
            ha="center",
            fontsize=10,
            color=sns.color_palette("pastel")[3],
        )

plt.tight_layout()
output_img_path = f"outputs/cache_hit_rate_comparison_n{num_accesses}_s{cache_size}_p{access_pattern}_a{associativity}.png"
plt.savefig(output_img_path, dpi=300, bbox_inches="tight")
print(f"图表已保存至: {output_img_path}")
plt.show()
