import numpy as np
import matplotlib.pyplot as plt

fs = 48000
N = 10
f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

# 前馈梳状 H(z) = 1 + g * z^(-N)
# |H| = sqrt(1 + g^2 + 2g*cos(omega*N))
H_ff_p1 = 1 + 1 * np.exp(-1j * omega * N)       # g = +1
H_ff_n1 = 1 + (-1) * np.exp(-1j * omega * N)    # g = -1
mag_ff_p1 = 20 * np.log10(np.abs(H_ff_p1) + 1e-12)
mag_ff_n1 = 20 * np.log10(np.abs(H_ff_n1) + 1e-12)

# 反馈梳状 H(z) = z^(-M) / (1 - g * z^(-M)),  g=0.8
M = 8
g_fb = 0.8
omega_fb = 2 * np.pi * f / fs
H_fb = np.exp(-1j * omega_fb * M) / (1 - g_fb * np.exp(-1j * omega_fb * M))
mag_fb = 20 * np.log10(np.abs(H_fb) + 1e-12)

fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))

# ----- 前馈 g = +1 -----
f0 = fs / N
for k in range(int(fs/2/f0) + 2):
    x = k * f0 + f0/2
    ax1.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.4)
ax1.plot(f, mag_ff_p1, 'b-', lw=1.5)
ax1.set_title(r'Feedforward  $H(z) = 1 + z^{-N}$  (g=+1)  $\Rightarrow$  $2|\cos(\omega N/2)|$')
ax1.set_ylabel('Magnitude (dB)')
ax1.set_xlim(0, fs/2)
ax1.set_ylim(-60, 10)
ax1.grid(alpha=0.3)
ax1.text(fs/2*0.98, -55, f'N={N},  notch spacing={fs/N:.0f} Hz', ha='right', fontsize=9, color='gray')

# ----- 前馈 g = -1 -----
for k in range(int(fs/2/f0) + 2):
    x = k * f0
    ax2.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.4)
ax2.plot(f, mag_ff_n1, 'b-', lw=1.5)
ax2.set_title(r'Feedforward  $H(z) = 1 - z^{-N}$  (g=-1)  $\Rightarrow$  $2|\sin(\omega N/2)|$')
ax2.set_ylabel('Magnitude (dB)')
ax2.set_xlim(0, fs/2)
ax2.set_ylim(-60, 10)
ax2.grid(alpha=0.3)

# ----- 反馈 g = 0.8 -----
f0_fb = fs / M
peak_db = 20 * np.log10(1 / (1 - g_fb))
valley_db = 20 * np.log10(1 / (1 + g_fb))

for k in range(int(fs/2/f0_fb) + 2):
    x = k * f0_fb
    ax3.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.4)
ax3.plot(f, mag_fb, 'r-', lw=1.5)
ax3.axhline(peak_db, color='green', ls='--', lw=0.8, alpha=0.6, label=f'peak={peak_db:.1f} dB')
ax3.axhline(valley_db, color='orange', ls='--', lw=0.8, alpha=0.6, label=f'valley={valley_db:.1f} dB')
ax3.set_title(r'Feedback  $H(z) = z^{-M} / (1 - g\,z^{-M})$  (M=8, g=0.8)')
ax3.set_xlabel('Frequency (Hz)')
ax3.set_ylabel('Magnitude (dB)')
ax3.set_xlim(0, fs/2)
ax3.set_ylim(valley_db - 6, peak_db + 6)
ax3.grid(alpha=0.3)
ax3.legend(fontsize=8)

plt.tight_layout()
plt.savefig('comb_comparison.png', dpi=150)
print("Done: comb_comparison.png")
