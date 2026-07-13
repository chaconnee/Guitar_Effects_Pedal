import numpy as np
import matplotlib.pyplot as plt

fs = 48000
N = 8
f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

f0 = fs / N
for k in range(int(fs/2/f0) + 2):
    x = k * f0
    ax1.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.3)
    ax2.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.3)

# ---- 上：g = ±1（无限深陷波） ----
for g, ls, color, label in [(-1, '-', 'b', r'$g=-1$:  $H(z)=1-z^{-N}$'),
                              (1, '--', 'c', r'$g=+1$:  $H(z)=1+z^{-N}$')]:
    H = 1 + g * np.exp(-1j * omega * N)
    mag = 20 * np.log10(np.abs(H) + 1e-12)
    ax1.plot(f, mag, ls=ls, color=color, lw=1.5, label=label)

ax1.set_ylim(-70, 10)
ax1.set_ylabel('Magnitude (dB)')
ax1.set_title(r'Zero on unit circle  $|g|=1$  $\Rightarrow$ infinite notch depth', fontsize=12)
ax1.legend(fontsize=10)
ax1.grid(alpha=0.3)

# ---- 下：|g| ≠ 1（有限深凹口） ----
for g, ls, color in [(0.5, '-', 'orange'), (0.9, '--', 'red'), (1.5, '-.', 'purple')]:
    H = 1 - g * np.exp(-1j * omega * N)
    mag = 20 * np.log10(np.abs(H) + 1e-12)
    notch_db = 20 * np.log10(abs(1 - abs(g)))
    ax2.plot(f, mag, ls=ls, color=color, lw=1.5,
             label=rf'$g={g}$: notch depth = {notch_db:.1f} dB')

ax2.set_ylim(-30, 10)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.set_title(r'Zero inside/outside unit circle  $|g| \neq 1$  $\Rightarrow$ finite notch depth', fontsize=12)
ax2.legend(fontsize=10)
ax2.grid(alpha=0.3)

plt.tight_layout()
plt.savefig('comb_g_comparison.png', dpi=150)
print("Done: comb_g_comparison.png")
