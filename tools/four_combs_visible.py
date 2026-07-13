import numpy as np
import matplotlib.pyplot as plt

fs = 32000
fb = 0.8
cb_lens = [2214, 1912, 2588, 2875]

f = np.linspace(0, 300, 5000)
omega = 2 * np.pi * f / fs

fig, ax = plt.subplots(figsize=(14, 6))

combined = np.zeros(len(f), dtype=complex)

for M, color in zip(cb_lens, ['tab:blue', 'tab:orange', 'tab:green', 'tab:red']):
    H = np.exp(-1j * omega * M) / (1 - fb * np.exp(-1j * omega * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    combined += H
    ax.plot(f, mag_db, color=color, lw=1.0, alpha=0.8,
            label=rf'M={M}  $f_0={fs/M:.1f}$ Hz')

peak_db = 20 * np.log10(1 / (1 - fb))
valley_db = 20 * np.log10(1 / (1 + fb))

ax.axhline(peak_db, color='gray', ls=':', lw=0.8, alpha=0.5,
           label=f'peak={peak_db:.1f} dB')
ax.axhline(valley_db, color='gray', ls='--', lw=0.8, alpha=0.5,
           label=f'valley={valley_db:.1f} dB')

ax.set_xlim(0, 200)
ax.set_ylim(valley_db - 6, peak_db + 6)
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Magnitude (dB)')
ax.set_title(f'Four Parallel Feedback Comb Filters  (fb={fb})', fontsize=12)
ax.grid(alpha=0.25)
ax.legend(fontsize=9)

plt.tight_layout()
plt.savefig('four_combs_visible.png', dpi=150)
print("Done: four_combs_visible.png")
