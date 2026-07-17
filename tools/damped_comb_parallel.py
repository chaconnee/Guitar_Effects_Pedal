import numpy as np
import matplotlib.pyplot as plt

fs = 32000
decay = 0.8
tone_val = 0.3

cb_lens = [2214, 1912, 2588, 2875]

damp = 0.1 + tone_val * 0.85
fb = 0.5 + decay * 0.42

print(f"decay={decay} -> fb={fb:.4f}")
print(f"tone={tone_val} -> damping={damp:.4f}")
print()

f = np.linspace(20, fs/2, 10000)
omega = 2 * np.pi * f / fs

H_lp = damp / (1 - (1 - damp) * np.exp(-1j * omega))

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))

combined = np.zeros(len(f), dtype=complex)

for idx, M in enumerate(cb_lens):
    G = fb * H_lp
    H = np.exp(-1j * omega * M) / (1 - G * np.exp(-1j * omega * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    combined += H

    f0 = fs / M
    ax1.plot(f, mag_db, lw=0.8, alpha=0.8, label=f'CB{idx}  M={M}  f0={f0:.1f} Hz')
    print(f"CB{idx}: M={M:4d},  f0={f0:6.1f} Hz")

combined_db = 20 * np.log10(np.abs(combined / 4) + 1e-12)
ax1.plot(f, combined_db, 'k-', lw=2.0, label='Sum / 4 (combined)')

ax1.set_xlim(0, 5000)
ax1.set_ylim(-50, 30)
ax1.set_xlabel('Frequency (Hz)')
ax1.set_ylabel('Magnitude (dB)')
ax1.set_title(f'4 Parallel Damped Comb Filters  (fb={fb:.3f}, damp={damp:.3f})  |  0-5 kHz')
ax1.grid(alpha=0.2)
ax1.legend(fontsize=8, ncol=2)

# 放大低频区看齿的细节
ax1ins = ax1.inset_axes([0.55, 0.55, 0.4, 0.4])
ax1ins.set_xlim(0, 200)
ax1ins.set_ylim(-30, 30)
for idx, M in enumerate(cb_lens):
    G = fb * H_lp
    H = np.exp(-1j * omega * M) / (1 - G * np.exp(-1j * omega * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    ax1ins.plot(f, mag_db, lw=0.6, alpha=0.6)
combined_db = 20 * np.log10(np.abs(combined / 4) + 1e-12)
ax1ins.plot(f, combined_db, 'k-', lw=1.5)
ax1ins.grid(alpha=0.2)
ax1ins.set_title('0-200 Hz (zoomed)', fontsize=9)

# ---- 底部：不同 damping 对比 ----
for damp_val, ls, color in [(0.1, '-', 'b'), (0.5, '--', 'orange'), (0.95, '-.', 'r')]:
    H_lp_d = damp_val / (1 - (1 - damp_val) * np.exp(-1j * omega))
    H = np.exp(-1j * omega * cb_lens[0]) / (1 - fb * H_lp_d * np.exp(-1j * omega * cb_lens[0]))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    ax2.plot(f, mag_db, ls=ls, color=color, lw=1.2, label=f'damping={damp_val}')

ax2.set_xlim(0, 5000)
ax2.set_ylim(-50, 30)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.set_title(f'Effect of damping on CB0 (M={cb_lens[0]}, fb={fb:.3f})')
ax2.grid(alpha=0.2)
ax2.legend(fontsize=9)

plt.tight_layout()
plt.savefig('damped_comb_parallel.png', dpi=150)
print("\nDone: damped_comb_parallel.png")
