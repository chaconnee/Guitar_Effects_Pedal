import numpy as np
import matplotlib.pyplot as plt

fs = 48000
N = 12
f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

colors = plt.cm.viridis(np.linspace(0.2, 0.9, 5))

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

# ===== 上：g = ±1 =====
for k in range(int(fs/2/(fs/N)) + 2):
    ax1.axvline(k * fs / N, color='gray', ls=':', lw=0.5, alpha=0.4)

for g, ls, label in [(-1, '-', r'$g = -1$:  $1 - z^{-N}$'),
                      (1, '--', r'$g = +1$:  $1 + z^{-N}$')]:
    H = 1 + g * np.exp(-1j * omega * N)
    ax1.plot(f, 20 * np.log10(np.abs(H) + 1e-12), ls=ls, lw=1.5, label=label)

ax1.set_title(r'Feedforward Comb Filter  $H(z) = 1 + g \cdot z^{-N}$  with $g = \pm 1$ (zeros on unit circle)')
ax1.set_ylabel('Magnitude (dB)')
ax1.set_xlim(0, fs/2)
ax1.set_ylim(-70, 10)
ax1.grid(alpha=0.3)
ax1.legend(fontsize=10)

# ===== 下：|g| ≠ 1 =====
for k in range(int(fs/2/(fs/N)) + 2):
    ax2.axvline(k * fs / N, color='gray', ls=':', lw=0.5, alpha=0.4)

for i, g in enumerate([0.3, 0.6, 0.9, 1.5]):
    H = 1 - g * np.exp(-1j * omega * N)
    ax2.plot(f, 20 * np.log10(np.abs(H) + 1e-12), color=colors[i], lw=1.5,
             label=rf'$g = {g}$')

ax2.set_title(r'$H(z) = 1 - g \cdot z^{-N}$  with $|g| \neq 1$ (zeros inside/outside unit circle)')
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.set_xlim(0, fs/2)
ax2.set_ylim(-30, 20)
ax2.grid(alpha=0.3)
ax2.legend(fontsize=10)

plt.tight_layout()
plt.savefig('ff_comb_g_comparison.png', dpi=150)
