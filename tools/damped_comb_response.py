import numpy as np
import matplotlib.pyplot as plt

fs = 32000
fb = 0.8
alpha = 0.35
cb_lens = [2214, 1912, 2588, 2875]

f = np.linspace(20, fs/2, 10000)
omega = 2 * np.pi * f / fs

# LPF inside feedback loop
H_lp = alpha / (1 - (1 - alpha) * np.exp(-1j * omega))

fig, ax = plt.subplots(figsize=(14, 6))

combined = np.zeros(len(f), dtype=complex)

for M, color in zip(cb_lens, ['tab:blue', 'tab:orange', 'tab:green', 'tab:red']):
    G = fb * H_lp
    H = np.exp(-1j * omega * M) / (1 - G * np.exp(-1j * omega * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    combined += H
    ax.plot(f, mag_db, color=color, lw=0.7, alpha=0.5,
            label=rf'CB M={M}')

combined_db = 20 * np.log10(np.abs(combined / 4) + 1e-12)
ax.plot(f, combined_db, 'k-', lw=2.0, label='Sum / 4 (combined)')

# 无阻尼单路峰值/谷值参考线
peak_nd = 20 * np.log10(1 / (1 - fb))
valley_nd = 20 * np.log10(1 / (1 + fb))
ax.axhline(peak_nd, color='gray', ls=':', lw=0.8, alpha=0.3,
           label=f'single peak (no LPF) = {peak_nd:.1f} dB')
ax.axhline(valley_nd, color='gray', ls='--', lw=0.8, alpha=0.3,
           label=f'single valley (no LPF) = {valley_nd:.1f} dB')

# 标注 LPF 截止区
ax.axvspan(5000, fs/2, color='lightcoral', alpha=0.08)
ax.text(fs/2 * 0.85, -35, 'LPF roll-off\nregion', fontsize=9,
        color='crimson', ha='center')

ax.set_xlim(0, fs/2)
ax.set_ylim(-50, 30)
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Magnitude (dB)')
ax.set_title(f'4 Parallel Damped Comb Filters  (fb={fb}, $\\alpha$={alpha})', fontsize=12)
ax.grid(alpha=0.2)
ax.legend(fontsize=8, ncol=2)

# 放大低频区
axins = ax.inset_axes([0.55, 0.55, 0.4, 0.35])
axins.set_xlim(0, 300)
axins.set_ylim(-25, 25)
for M, color in zip(cb_lens, ['tab:blue', 'tab:orange', 'tab:green', 'tab:red']):
    G = fb * H_lp
    H = np.exp(-1j * omega * M) / (1 - G * np.exp(-1j * omega * M))
    axins.plot(f, 20*np.log10(np.abs(H)+1e-12), color=color, lw=0.6, alpha=0.5)
axins.plot(f, combined_db, 'k-', lw=1.5)
axins.grid(alpha=0.2)
axins.set_title('0-300 Hz (zoomed)', fontsize=9)

plt.tight_layout()
plt.savefig('damped_comb_full_response.png', dpi=150)
print("Done: damped_comb_full_response.png")
