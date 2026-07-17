import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

fig, ax = plt.subplots(figsize=(16, 9))
ax.set_xlim(0, 20)
ax.set_ylim(0, 10)
ax.axis('off')

def box(x, y, w, h, text, color='lightblue', ec='black'):
    r = mpatches.FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.15",
                                 facecolor=color, edgecolor=ec, lw=1.5)
    ax.add_patch(r)
    ax.text(x + w/2, y + h/2, text, ha='center', va='center', fontsize=8, fontfamily='monospace')

def arrow(x1, y1, x2, y2, **kw):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='->', lw=1.5, **kw), annotation_clip=False)

def label(x, y, text, fs=8, **kw):
    ax.text(x, y, text, ha='center', va='center', fontsize=fs, fontfamily='monospace', **kw)

# ── 输入 ──
label(1.0, 7.5, 'Input', fs=11, weight='bold')
arrow(1.8, 7.5, 3.0, 7.5)

# ── 四条并联分支 ──
y_pos = [8.2, 7.5, 6.8, 6.1]
colors = ['#cce5ff', '#d4edda', '#fff3cd', '#f8d7da']

# 汇聚线
arrow(3.0, 7.5, 3.5, 8.2); arrow(3.0, 7.5, 3.5, 7.5)
arrow(3.0, 7.5, 3.5, 6.8); arrow(3.0, 7.5, 3.5, 6.1)

for i, (y, c) in enumerate(zip(y_pos, colors)):
    bx = 3.5
    # 加法器
    ax.plot(bx + 0.7, y, 'o', markersize=8, color='white', markeredgecolor='black', markeredgewidth=1.5, zorder=3)
    ax.text(bx + 0.7, y, '+', ha='center', va='center', fontsize=12, weight='bold')

    # 延迟 M
    box(bx + 1.2, y - 0.3, 1.2, 0.6, f'z^(-M)\nM={[2214,1912,2588,2875][i]}', '#e8daef')
    arrow(bx + 0.7, y, bx + 1.2, y)

    # 反馈路径
    ax.plot(bx + 1.2 + 1.2 + 0.7, y, 'o', markersize=8, color='white', markeredgecolor='black', markeredgewidth=1.5, zorder=3)
    ax.text(bx + 1.2 + 1.2 + 0.7, y, '+', ha='center', va='center', fontsize=12, weight='bold')

    # 反馈增益 ×g
    box(bx + 0.4, y - 1.5, 0.8, 0.5, f'×{0.5+0.8*0.42:.2f}', '#ffe6cc')
    arrow(bx + 0.7, y - 0.3, bx + 0.7, y - 1.0)

    # 低通 LPF
    box(bx + 0.4, y - 2.4, 0.8, 0.5, 'LPF\nα=0.35', '#d5f5e3')
    arrow(bx + 0.7, y - 1.5, bx + 0.7, y - 2.4)

    # 从延迟输出往下到反馈
    arrow(bx + 2.4, y, bx + 2.4, y - 2.8); arrow(bx + 2.4, y - 2.8, bx + 1.2, y - 2.8)
    arrow(bx + 1.2, y - 2.8, bx + 0.8, y - 2.4)

    # 输出
    arrow(bx + 3.0, y, bx + 3.8, y)

# ── 求和(÷4) ──
sum_x = 10.2
for y in y_pos:
    arrow(7.3, y, sum_x - 0.2, 7.5)

ax.plot(sum_x, 7.5, 'o', markersize=14, color='white', markeredgecolor='black', markeredgewidth=2, zorder=3)
ax.text(sum_x, 7.5, '+\n÷4', ha='center', va='center', fontsize=10, weight='bold')
arrow(sum_x + 0.3, 7.5, sum_x + 1.2, 7.5)

# ── 三个串联全通 ──
ap_x = [11.8, 14.2, 16.6]
ap_Ms = [320, 108, 31]
for i, (x, M) in enumerate(zip(ap_x, ap_Ms)):
    box(x, 7.0, 1.6, 1.0, f'Allpass\nM={M}  g=0.7', '#d1f2eb')
    if i == 0:
        arrow(sum_x + 1.2, 7.5, x, 7.5)
    else:
        arrow(ap_x[i-1] + 1.6, 7.5, x, 7.5)
    if i == 2:
        arrow(x + 1.6, 7.5, 19.0, 7.5)

# ── 干湿混合 ──
label(19.5, 7.5, 'Wet', fs=11, weight='bold')

# 干路
arrow(1.8, 7.5, 1.8, 0.5)
arrow(1.8, 0.5, 19.0, 0.5)
box(17.8, 0.0, 1.6, 1.0, 'Dry\n×(1-mix)', '#f9e79f')
arrow(19.0, 0.5, 19.0, 1.0)

# 湿路
box(17.8, 7.0, 1.6, 1.0, 'Wet\n×mix', '#f9e79f')
arrow(17.8, 7.5, 16.6, 7.5)

# 最终加法器
ax.plot(19.5, 0.5, 'o', markersize=10, color='white', markeredgecolor='black', markeredgewidth=2, zorder=3)
ax.text(19.5, 0.5, '+', ha='center', va='center', fontsize=14, weight='bold')
arrow(19.5, 1.0, 19.5, 0.8)
arrow(19.5, 0.5, 20.5, 0.5)
label(21.0, 0.5, 'Output', fs=11, weight='bold')

plt.tight_layout()
plt.savefig('schroeder_topology.png', dpi=200)
print("Done: schroeder_topology.png")
