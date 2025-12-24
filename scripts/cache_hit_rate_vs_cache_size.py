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
# 选取6个典型的缓存大小: 32KB, 64KB, 128KB, 256KB, 512KB, 1MB
cache_sizes = [32 * 1024, 64 * 1024, 128 * 1024, 256 * 1024, 512 * 1024, 1024 * 1024]
access_pattern = "localized"
associativity = 4
ws_step = 2000
ws_periods = range(ws_step, num_accesses, ws_step) 
fixed_ws_size = 128 * 1024 

executable_path = "./build/cache_sim"
working_dir = ".."

results = []

print(f"正在运行模拟...")
print(f"当前目录: {os.path.abspath(working_dir)}")

for size in cache_sizes:
    print(f"正在测试缓存大小: {size / 1024:.0f} KB")
    for i, ws in enumerate(ws_periods):
        # LRU 模拟
        cmd_lru = [
            executable_path,
            "-n", str(num_accesses),
            "-s", str(size),
            "-t", access_pattern,
            "-a", str(associativity),
            "-w", str(ws),
            "-v", str(fixed_ws_size),
            "-p", "lru",
            "-j"
        ]
        
        try:
            result_lru = subprocess.run(cmd_lru, capture_output=True, text=True, check=True, cwd=working_dir)
            data_lru = json.loads(result_lru.stdout)
            lru_hit_rate = data_lru['average']['hit_rate']

            results.append({
                "cache_size": size,
                "ws_period": ws,
                "hit_rate": lru_hit_rate
            })
        except subprocess.CalledProcessError as e:
            print(f"运行模拟出错 size {size}, ws {ws}: {e}")
        except json.JSONDecodeError as e:
            print(f"JSON解码错误 size {size}, ws {ws}: {e}")

df = pd.DataFrame(results)
print("数据形状:", df.shape)

# 保存CSV (原笔记本未保存，这里补充)
output_csv_path = f"outputs/cache_hit_rate_by_size_n{num_accesses}_a{associativity}_p{access_pattern}.csv"
df.to_csv(output_csv_path, index=False)
print(f"数据已保存至: {output_csv_path}")

# 绘图
plt.rcParams["font.sans-serif"] = ["LXGW ZhenKai GB", "SimHei", "DejaVu Sans"]

fig, ax1 = plt.subplots(1, 1, figsize=(10, 7))

# 获取唯一的缓存大小并设置颜色
sizes = sorted(df["cache_size"].unique())
palette = sns.color_palette("flare", len(sizes))

for i, size in enumerate(sizes):
    subset = df[df["cache_size"] == size]
    
    if len(subset) < 4: # 点数太少无法进行样条拟合
        ax1.plot(subset["ws_period"], subset["hit_rate"], marker='o', label=f"{size/1024:.0f} KB", color=palette[i])
        continue

    # 原始数据点 (可选，这里只画线保持整洁)
    ax1.plot(subset["ws_period"], subset["hit_rate"], marker="o", markevery=2, linewidth=0, markersize=3, color=palette[i], alpha=0.5)

    # 样条曲线拟合
    try:
        spl = UnivariateSpline(subset["ws_period"], subset["hit_rate"], s=10)
        
        # 生成平滑曲线
        x_smooth = np.linspace(subset["ws_period"].min(), subset["ws_period"].max(), 200)
        y_smooth = spl(x_smooth)
        y_smooth = np.clip(y_smooth, 0, 100) # 限制在 0-100 之间

        # 绘制平滑曲线
        ax1.plot(
            x_smooth,
            y_smooth,
            linewidth=2,
            label=f"{size/1024:.0f} KB",
            color=palette[i],
        )
    except Exception as e:
        print(f"样条拟合失败 size {size}: {e}")
        ax1.plot(subset["ws_period"], subset["hit_rate"], label=f"{size/1024:.0f} KB", color=palette[i])


ax1.set_xlabel("工作集周期 Work Set Period")
ax1.set_ylabel("命中率 Hit Rate (%)")
ax1.set_title(f"不同缓存大小下命中率随工作集周期的变化\n（LRU，{associativity}-way，{access_pattern}，工作集大小 {fixed_ws_size/1024:.0f} KB）")
ax1.legend(title="Cache Size")
ax1.grid(True, alpha=0.3)

plt.tight_layout()
output_img_path = f"outputs/cache_hit_rate_by_size_n{num_accesses}_a{associativity}_p{access_pattern}.png"
plt.savefig(output_img_path, dpi=300, bbox_inches="tight")
print(f"图表已保存至: {output_img_path}")
plt.show()
