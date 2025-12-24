import subprocess
import json
import pandas as pd
import os
from matplotlib import pyplot as plt
import seaborn as sns

# 确保输出目录存在
os.makedirs("outputs", exist_ok=True)

# 配置参数
num_accesses = 500000  # 增加访问次数以预热
# 调整缓存大小范围，覆盖工作集大小 (64KB) 和地址范围 (256KB)
cache_sizes_kb = [4, 16, 32, 64, 128, 256, 512]
cache_sizes = [size * 1024 for size in cache_sizes_kb]

access_patterns = ["random", "sequential", "localized"]
associativity = 4
policy = "lru"
ws_period = 50000
ws_size = 64 * 1024        # 64KB 局部性工作集
address_range = 256 * 1024 # 256KB 总地址范围 (用于 Random/Sequential)

executable_path = "./build/cache_sim"
working_dir = ".."

results = []

print(f"正在运行模拟...")
print(f"当前目录: {os.path.abspath(working_dir)}")

for pattern in access_patterns:
    print(f"正在测试访问模式: {pattern}")
    for size in cache_sizes:
        # print(f"  Cache Size: {size} bytes")
        
        cmd = [
            executable_path,
            "-n", str(num_accesses),
            "-s", str(size),
            "-t", pattern,
            "-a", str(associativity),
            "-p", policy,
            "-w", str(ws_period),
            "-v", str(ws_size),
            "-r", str(address_range), # 显式设置地址范围
            "-j"
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, check=True, cwd=working_dir)
            data = json.loads(result.stdout)
            hit_rate = data['average']['hit_rate']
            
            results.append({
                "access_pattern": pattern,
                "cache_size_bytes": size,
                "cache_size_kb": size / 1024,
                "hit_rate": hit_rate
            })
        except subprocess.CalledProcessError as e:
            print(f"运行模拟出错 size={size}, pattern={pattern}: {e}")

df = pd.DataFrame(results)
base_filename = f"outputs/cache_hit_rate_vs_access_pattern_n{num_accesses}_a{associativity}"
output_csv_path = f"{base_filename}.csv"
df.to_csv(output_csv_path, index=False)
print(f"数据已保存至: {output_csv_path}")

# 绘图
plt.rcParams["font.sans-serif"] = ["LXGW ZhenKai GB", "SimHei", "DejaVu Sans"] # 添加备用字体
plt.figure(figsize=(10, 6))

sns.lineplot(
    data=df,
    x="cache_size_kb",
    y="hit_rate",
    hue="access_pattern",
    marker="o",
    linewidth=2,
    markersize=8
)

plt.xscale("log")
plt.xticks(cache_sizes_kb, [f"{s}K" for s in cache_sizes_kb])
plt.xlabel("缓存大小 Cache Size (KB)")
plt.ylabel("命中率 Hit Rate (%)")
plt.title(f"不同访问模式的命中率随缓存大小变化\n(LRU, {associativity}-way, Range={address_range//1024}KB, WS={ws_size//1024}KB)")
plt.grid(True, which="both", linestyle="--", alpha=0.5)

plt.tight_layout()
output_img_path = f"{base_filename}.png"
plt.savefig(output_img_path, dpi=300)
print(f"图表已保存至: {output_img_path}")
# plt.show()
