import numpy as np
import matplotlib.pyplot as plt

fs = 32000
g  = 0.7
Ms = [31, 108, 320]

f = np.linspace(20, fs/2, 10000)
omega = 2 * np.pi * f / fs

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

total_H = np.ones(len(f), dtype=complex)

for M, color in zip(Ms, ['tab:orange', 'tab:green', 'tab:blue']):
    z = np.exp(1j * omega)
    H = (z**(-M) - g) / (1 - g * z**(-M))
    phase = np.unwrap(np.angle(H)) * 180 / np.pi
    total_H *= H
    ax1.plot(f, phase, color=color, lw=1.2, label=rf'M={M}  (len={M})')

total_phase = np.unwrap(np.angle(total_H)) * 180 / np.pi
ax1.plot(f, total_phase, 'k-', lw=2.0, label='Total (series)')

ax1.set_xlim(0, 5000)
ax1.set_xlabel('Frequency (Hz)')
ax1.set_ylabel('Phase (deg, unwrapped)')
ax1.set_title('Allpass Phase Response  (individual + series)', fontsize=12)
ax1.grid(alpha=0.2)
ax1.legend(fontsize=9)

# 放大低频0-500 Hz
ax1ins = ax1.inset_axes([0.5, 0.5, 0.45, 0.45])
for M, color in zip(Ms, ['tab:orange', 'tab:green', 'tab:blue']):
    z = np.exp(1j * omega)
    H = (z**(-M) - g) / (1 - g * z**(-M))
    phase = np.unwrap(np.angle(H)) * 180 / np.pi
    ax1ins.plot(f, phase, color=color, lw=0.8)
ax1ins.plot(f, total_phase, 'k-', lw=1.5)
ax1ins.set_xlim(0, 500)
ax1ins.set_title('0-500 Hz (zoomed)', fontsize=9)
ax1ins.grid(alpha=0.2)

# ── 群延迟 ──
gd = -np.gradient(np.unwrap(np.angle(total_H)), omega)

ax2.plot(f[1:], gd[1:], 'k-', lw=1.5)
for M, color in zip(Ms, ['tab:orange', 'tab:green', 'tab:blue']):
    z = np.exp(1j * omega)
    H = (z**(-M) - g) / (1 - g * z**(-M))
    gd_i = -np.gradient(np.unwrap(np.angle(H)), omega)
    ax2.plot(f[1:], gd_i[1:], color=color, lw=0.8, alpha=0.5)

ax2.set_xlim(0, 5000)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Group Delay (samples)')
ax2.set_title('Group Delay — total series allpass', fontsize=12)
ax2.grid(alpha=0.2)

avg_gd = np.mean(gd[1:])
ax2.axhline(avg_gd, color='red', ls='--', lw=0.8, alpha=0.6,
            label=f'Mean gd ≈ {avg_gd:.0f} samples')
ax2.legend(fontsize=9)

plt.tight_layout()
plt.savefig('allpass_series.png', dpi=150)
print("Done: allpass_series.png")
print(f"Total phase wrap at fs/2 ≈ {total_phase[-1]:.0f} deg")
print(f"Mean group delay ≈ {avg_gd:.1f} samples = {avg_gd/fs*1000:.2f} ms")
