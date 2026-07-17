import numpy as np
import matplotlib.pyplot as plt

fs = 32000
f = np.linspace(20, fs/2, 5000)
omega = 2 * np.pi * f / fs

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 9))

# ────────────────────────────────────────────
# 1. 一阶全通  H(z) = (z^{-1} - g) / (1 - g·z^{-1})
# ────────────────────────────────────────────
for g in [0.3, 0.6, 0.9]:
    z = np.exp(1j * omega)
    H = (z**(-1) - g) / (1 - g * z**(-1))
    mag = 20 * np.log10(np.abs(H) + 1e-15)
    phase = np.unwrap(np.angle(H))
    ax1.plot(f, mag, lw=1.5, label=rf'1st-order  g={g}')
    ax2.plot(f, phase * 180 / np.pi, lw=1.5, label=rf'1st-order  g={g}')

# ────────────────────────────────────────────
# 2. 二阶全通  H(z) = (r² - 2r·cosθ·z^{-1} + z^{-2})
#                         / (1 - 2r·cosθ·z^{-1} + r²·z^{-2})
# ────────────────────────────────────────────
for r, theta in [(0.7, 0.3*np.pi), (0.9, 0.5*np.pi)]:
    z = np.exp(1j * omega)
    num = r**2 - 2*r*np.cos(theta)*z**(-1) + z**(-2)
    den = 1 - 2*r*np.cos(theta)*z**(-1) + r**2 * z**(-2)
    H = num / den
    mag = 20 * np.log10(np.abs(H) + 1e-15)
    phase = np.unwrap(np.angle(H))
    ax1.plot(f, mag, ls='--', lw=1.5,
             label=rf'2nd-order  r={r}, θ={theta/np.pi:.1f}π')
    ax2.plot(f, phase * 180 / np.pi, ls='--', lw=1.5,
             label=rf'2nd-order  r={r}, θ={theta/np.pi:.1f}π')

# ────────────────────────────────────────────
# 3. 长延迟全通（混响代码） H(z) = (z^{-M} - g) / (1 - g·z^{-M})
# ────────────────────────────────────────────
for M, color in [(31, 'tab:purple'), (108, 'tab:brown'), (320, 'tab:pink')]:
    g = 0.7
    z = np.exp(1j * omega)
    H = (z**(-M) - g) / (1 - g * z**(-M))
    mag = 20 * np.log10(np.abs(H) + 1e-15)
    phase = np.unwrap(np.angle(H))
    ax1.plot(f, mag, ls='-.', lw=1.0, alpha=0.7, color=color,
             label=rf'long-delay  M={M}, g={g}')
    ax2.plot(f, phase * 180 / np.pi, ls='-.', lw=1.0, alpha=0.7, color=color,
             label=rf'long-delay  M={M}, g={g}')

ax1.set_ylabel('Magnitude (dB)')
ax1.set_title('Allpass Filter Magnitude Response  (all should be 0 dB)', fontsize=11)
ax1.set_xlim(20, fs/2)
ax1.set_ylim(-0.5, 0.5)
ax1.grid(alpha=0.2)
ax1.legend(fontsize=7, ncol=2)

ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Phase (deg, unwrapped)')
ax2.set_title('Allpass Filter Phase Response', fontsize=11)
ax2.set_xlim(20, fs/2)
ax2.grid(alpha=0.2)
ax2.legend(fontsize=7, ncol=2)

plt.tight_layout()
plt.savefig('allpass_comparison.png', dpi=150)
print("Done: allpass_comparison.png")
