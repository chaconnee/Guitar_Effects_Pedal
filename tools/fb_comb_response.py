import numpy as np
import matplotlib.pyplot as plt

fs = 48000
M = 8
g = 0.8

f0 = fs / M

f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

# 反馈梳状 H(z) = z^(-M) / (1 - g·z^(-M))
# |H| = 1 / sqrt(1 + g^2 - 2g·cos(ωM))
mag = 1 / np.sqrt(1 + g**2 - 2*g*np.cos(omega * M))
mag_db = 20 * np.log10(mag)

peak_db = 20 * np.log10(1 / (1 - g))
valley_db = 20 * np.log10(1 / (1 + g))

fig, ax = plt.subplots(figsize=(12, 5))

for k in range(int(fs/2/f0) + 2):
    x = k * f0
    ax.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.4)
    ax.axvline(x + f0/2, color='gray', ls='--', lw=0.5, alpha=0.3)

ax.plot(f, mag_db, 'r-', lw=1.5)
ax.axhline(peak_db, color='green', ls='--', lw=0.8, alpha=0.7,
           label=f'Peak = {peak_db:.1f} dB  (1/(1-g))')
ax.axhline(valley_db, color='orange', ls='--', lw=0.8, alpha=0.7,
           label=f'Valley = {valley_db:.1f} dB  (1/(1+g))')

ax.set_title(f'Feedback Comb Filter  $H(z) = z^{{-M}} / (1 - g z^{{-M}})$  (M={M}, g={g})')
ax.set_xlabel('Frequency (Hz)')
ax.set_ylabel('Magnitude (dB)')
ax.set_xlim(0, fs/2)
ax.set_ylim(valley_db - 6, peak_db + 6)
ax.grid(alpha=0.3)
ax.legend()

plt.tight_layout()
plt.savefig('fb_comb_response.png', dpi=150)
print(f"M = {M},  g = {g}")
print(f"Peak  @ f = k * {f0:.0f} Hz  ->  1/(1-g) = {1/(1-g):.2f}  ->  {peak_db:.1f} dB")
print(f"Valley @ f = (k+0.5) * {f0:.0f} Hz  ->  1/(1+g) = {1/(1+g):.2f}  ->  {valley_db:.1f} dB")
print(f"Comb spacing Δf = {f0:.0f} Hz")
