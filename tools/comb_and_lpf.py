import numpy as np
import matplotlib.pyplot as plt

fs = 32000
fb = 0.8
alpha = 0.35
cb_lens = [2214, 1912, 2588, 2875]

f1 = np.linspace(0, 500, 5000)
f2 = np.linspace(20, fs/2, 5000)
omega1 = 2 * np.pi * f1 / fs
omega2 = 2 * np.pi * f2 / fs

H_lp2 = alpha / (1 - (1 - alpha) * np.exp(-1j * omega2))

# 计算 -3 dB 截止频率
H_lp2_db = 20 * np.log10(np.abs(H_lp2) + 1e-12)
idx = np.argmin(np.abs(H_lp2_db + 3))
fc = f2[idx]

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

# ── 上图：四个并联梳状 + 合成（无 LPF） ──
f_comb = np.linspace(0, 500, 5000)
omega_comb = 2 * np.pi * f_comb / fs

combined = np.zeros(len(f_comb), dtype=complex)
for M, color in zip(cb_lens, ['tab:blue', 'tab:orange', 'tab:green', 'tab:red']):
    H = np.exp(-1j * omega_comb * M) / (1 - fb * np.exp(-1j * omega_comb * M))
    mag_db = 20 * np.log10(np.abs(H) + 1e-12)
    combined += H
    ax1.plot(f_comb, mag_db, color=color, lw=0.7, alpha=0.5, label=f'CB M={M}')

combined_db = 20 * np.log10(np.abs(combined / 4) + 1e-12)
ax1.plot(f_comb, combined_db, 'k-', lw=2.0, label='Sum / 4')

ax1.axhline(20*np.log10(1/(1-fb)), color='gray', ls=':', lw=0.8, alpha=0.3)
ax1.axhline(20*np.log10(1/(1+fb)), color='gray', ls='--', lw=0.8, alpha=0.3)
ax1.set_ylabel('Magnitude (dB)')
ax1.set_title('Four Parallel Combs (no LPF)  —  comb effect only', fontsize=11)
ax1.set_xlim(0, 500)
ax1.set_ylim(-30, 25)
ax1.grid(alpha=0.2)
ax1.legend(fontsize=7, ncol=2)

# ── 下图：LPF 幅频响应（半对数坐标） ──
ax2.semilogx(f2, H_lp2_db, 'b-', lw=2)
ax2.axhline(-3, color='red', ls='--', lw=0.8, alpha=0.6)
ax2.axvline(fc, color='red', ls=':', lw=0.8, alpha=0.5)
ax2.text(fc * 1.1, -4.5, f'fc  -3dB ≈ {fc:.0f} Hz', fontsize=8, color='red')

ax2.axvline(500, color='gray', ls=':', lw=0.8, alpha=0.3)
ax2.text(520, -2, 'previous X limit (500 Hz)', fontsize=8, color='gray', rotation=90)

ax2.set_ylabel('Magnitude (dB)')
ax2.set_xlabel('Frequency (Hz)')
ax2.set_title(r'One-pole LPF  $H_{lp}(z) = \dfrac{\alpha}{1-(1-\alpha)z^{-1}}$  ' +
              rf'($\alpha$ = {alpha})', fontsize=11)
ax2.set_xlim(20, fs/2)
ax2.set_ylim(-25, 3)
ax2.grid(alpha=0.2, which='both')

plt.tight_layout()
plt.savefig('comb_and_lpf_separate.png', dpi=150)
print("Done: comb_and_lpf_separate.png")
print(f"LPF -3dB fc ≈ {fc:.0f} Hz")
