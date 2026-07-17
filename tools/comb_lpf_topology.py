import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

fig, ax = plt.subplots(figsize=(10, 6))
ax.set_xlim(0, 14)
ax.set_ylim(0, 6)
ax.axis('off')

def box(x, y, w, h, text, color='#d5f5e3', ec='black'):
    r = mpatches.FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.12",
                                 facecolor=color, edgecolor=ec, lw=1.5)
    ax.add_patch(r)
    ax.text(x + w/2, y + h/2, text, ha='center', va='center', fontsize=9, fontfamily='monospace')

def arrow(x1, y1, x2, y2, **kw):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='->', lw=1.5, **kw), annotation_clip=False)

def lbl(x, y, text, fs=10, **kw):
    ax.text(x, y, text, ha='center', va='center', fontsize=fs, fontfamily='monospace', **kw)

# ── 主信号路径（从左到右） ──
lbl(0.8, 3.5, 'x[n]', fs=11, weight='bold')

# 加法器
ax.plot(2.2, 3.5, 'o', markersize=14, color='white', markeredgecolor='black', markeredgewidth=2, zorder=3)
ax.text(2.2, 3.5, '+', ha='center', va='center', fontsize=14, weight='bold')

arrow(1.3, 3.5, 1.8, 3.5)

# 延迟
box(3.2, 3.0, 1.6, 1.0, 'Delay\nz^(-M)', '#e8daef')
arrow(2.6, 3.5, 3.2, 3.5)

# 输出取点（在LPF之前）
ax.plot(5.2, 3.5, 'o', markersize=8, color='#2ecc71', markeredgecolor='black', markeredgewidth=2, zorder=3)
arrow(4.8, 3.5, 5.0, 3.5)
lbl(5.2, 3.5, '', fs=11)  # just marker
arrow(5.2, 3.5, 6.0, 3.5)
lbl(6.5, 3.5, 'y[n]', fs=11, weight='bold')

# ── feedback path ──

arrow(4.0, 3.0, 4.0, 0.8)

lbl(5.8, 1.7, 'readback = buf[ptr]', fs=9, color='#666')

# ② 再经过 LPF
box(3.2, 0.6, 1.6, 0.8, '2. LPF\nα = 0.35', '#d5f5e3')
arrow(4.0, 0.8, 4.0, 0.6)

# 标注 LPF 系数
lbl(6.2, 1.0, 'prev += a*(readback - prev)', fs=8, color='#888')

# ③ 再乘以反馈增益
box(3.2, 2.0, 1.6, 0.6, "3. x fb\n≈ 0.84", '#ffe6cc')
arrow(4.0, 1.4, 4.0, 2.0)
lbl(6.0, 2.3, 'buf[ptr] = x[n] + prev·fb', fs=8, color='#888')

# 反馈回到加法器
arrow(4.0, 2.6, 3.0, 3.1)

# ── 标注输出在 LPF 之前 ──
lbl(7.0, 3.5, '(output BEFORE LPF)', fs=8, color='#888')

# ── 右下信息框 ──
info = ("Signal flow:\n"
        "1. read buf[ptr] (delayed value)\n"
        "2. LPF filter the readback\n"
        "3. filtered result x fb\n"
        "4. add x[n], write to buf[ptr]\n"
        "5. output y[n] = raw readback (unfiltered)")
box(8.5, 0.5, 4.8, 1.8, info, '#fff9e6', '#ccc')

plt.tight_layout()
plt.savefig('comb_lpf_topology.png', dpi=200)
print("Done: comb_lpf_topology.png")
