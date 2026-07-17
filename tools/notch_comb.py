import numpy as np
import matplotlib.pyplot as plt

fs = 48000
N = 12
f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

H = 1 - np.exp(-1j * omega * N)
mag_db = 20 * np.log10(np.abs(H) + 1e-12)

fig, ax = plt.subplots(figsize=(12, 5))

f0 = fs / N
for k in range(int(fs/2/f0) + 2):
    x = k * f0
    ax.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.4)

ax.plot(f, mag_db, 'b-', lw=1.5, label=r'$|H(e^{j\omega})| = 2|\sin(\omega N/2)|$')
ax.set_title(r'Notch (Feedforward) Comb Filter  $H(z) = 1 - z^{-N}$', fontsize=13)
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Magnitude (dB)')
ax.set_xlim(0, fs/2)
ax.set_ylim(-70, 10)
ax.grid(alpha=0.3)
ax.legend(fontsize=10)

props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
ax.text(0.98, 0.95,
        rf'N = {N}\nfs = {fs} Hz\n$\Delta f = f_s/N = {fs/N:.0f}$ Hz\nNotch depth: $-\infty$ dB\nPeak: +6 dB',
        transform=ax.transAxes, fontsize=10, verticalalignment='top',
        horizontalalignment='right', bbox=props)

plt.tight_layout()
plt.savefig('notch_comb_filter.png', dpi=150)
print("Done: notch_comb_filter.png")
