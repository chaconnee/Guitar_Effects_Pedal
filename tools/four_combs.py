import numpy as np
import matplotlib.pyplot as plt

fs = 32000
fb = 0.8
cb_lens = [2214, 1912, 2588, 2875]

f = np.linspace(0, fs/2, 10000)
omega = 2 * np.pi * f / fs

# 纯反馈梳状（无阻尼低通） H(z) = z^{-M} / (1 - fb * z^{-M})
fig, ax1 = plt.subplots(figsize=(14, 5))

combined = np.zeros(len(f), dtype=complex)

for M in cb_lens:
    H = np.exp(-1j * omega * M) / (1 - fb * np.exp(-1j * omega * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    combined += H
    ax1.plot(f, mag_db, lw=0.8, alpha=0.7, label=f'M={M}')

ax1.set_xlim(0, 5000)
ax1.set_ylim(-50, 40)
ax1.set_xlabel('Frequency (Hz)')
ax1.set_ylabel('Magnitude (dB)')
ax1.set_title(f'Four Feedback Comb Filters  (fb={fb}, no damping)', fontsize=12)
ax1.grid(alpha=0.2)
ax1.legend(fontsize=8)

plt.tight_layout()
plt.savefig('four_combs_individual.png', dpi=150)

# ---- 合成响应 ----
fig2, ax2 = plt.subplots(figsize=(14, 5))

combined_db = 20 * np.log10(np.abs(combined / 4) + 1e-12)
ax2.plot(f, combined_db, 'k-', lw=1.5)
ax2.axhline(20 * np.log10(1/(1-fb)), color='green', ls='--', lw=0.8, alpha=0.5,
            label=f'Single peak = {20*np.log10(1/(1-fb)):.1f} dB')
ax2.axhline(20 * np.log10(1/(1+fb)), color='orange', ls='--', lw=0.8, alpha=0.5,
            label=f'Single valley = {20*np.log10(1/(1+fb)):.1f} dB')

ax2.set_xlim(0, 5000)
ax2.set_ylim(-50, 40)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.set_title(f'Combined (sum/4) Response of 4 Parallel Combs  (fb={fb}, no damping)', fontsize=12)
ax2.grid(alpha=0.2)
ax2.legend(fontsize=9)

plt.tight_layout()
plt.savefig('four_combs_combined.png', dpi=150)
print("Done: four_combs_individual.png + four_combs_combined.png")
