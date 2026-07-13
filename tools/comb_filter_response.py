import numpy as np
import matplotlib.pyplot as plt

fs = 48000
N = 10
f = np.linspace(0, fs/2, 5000)
omega = 2 * np.pi * f / fs

f0 = fs / N  # fundamental spacing

# 前馈梳状滤波器 H(z) = 1 - z^(-N)
H_ff = 1 - np.exp(-1j * omega * N)
mag_ff = 20 * np.log10(np.abs(H_ff) + 1e-12)

# 反馈梳状滤波器 H(z) = 1 / (1 - z^(-N))
H_fb = 1 / (1 - np.exp(-1j * omega * N))
mag_fb = 20 * np.log10(np.abs(H_fb) + 1e-12)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 7))

ax1.plot(f, mag_ff, 'b-', lw=1)
for n in range(int(fs/2/f0) + 2):
    x = n * f0
    ax1.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.5)
    ax2.axvline(x, color='gray', ls=':', lw=0.5, alpha=0.5)

ax1.plot(f, mag_ff, 'b-', lw=1.2)
ax1.set_title(f'Feedforward Comb Filter  $H(z) = 1 - z^{{-N}}$  (N={N}, no damping)')
ax1.set_ylabel('Magnitude (dB)')
ax1.grid(alpha=0.3)
ax1.set_xlim(0, fs/2)
ax1.set_ylim(-60, 6)

ax2.plot(f, mag_fb, 'r-', lw=1.2)
ax2.set_title(f'Feedback Comb Filter  $H(z) = 1/(1 - z^{{-N}})$  (N={N}, no damping)')
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Magnitude (dB)')
ax2.grid(alpha=0.3)
ax2.set_xlim(0, fs/2)
ax2.set_ylim(-30, 40)

plt.tight_layout()
plt.savefig('comb_filter_response.png', dpi=150)

print(f"Delay: {N/fs*1000:.1f} ms")
print(f"Fundamental frequency (notch/peak spacing): {fs/N:.1f} Hz")
print(f"Notch/peak frequencies: k * {f0:.0f} Hz for k = 1, 2, 3, ...")
