import numpy as np
import matplotlib.pyplot as plt

fs = 32000
f = np.linspace(20, fs/2, 2000)
omega = 2 * np.pi * f / fs

fig, ax = plt.subplots(figsize=(10, 5))

alpha_vals = [0.1, 0.3, 0.5, 0.7, 0.95]

for a in alpha_vals:
    H = a / (1 - (1 - a) * np.exp(-1j * omega))
    mag = 20 * np.log10(np.abs(H) + 1e-12)
    ax.semilogx(f, mag, lw=1.5, label=rf'$\alpha$ = {a}')

ax.set_xlim(20, fs/2)
ax.set_ylim(-40, 3)
ax.set_ylabel('Magnitude (dB)')
ax.set_xlabel('Frequency (Hz)')
ax.set_title(r'One-pole LPF  $H_{lp}(z) = \dfrac{\alpha}{1-(1-\alpha)z^{-1}}$')
ax.grid(alpha=0.3, which='both')
ax.legend(fontsize=9)

plt.tight_layout()
plt.savefig('lpf_response.png', dpi=150)

print(r"alpha = 0.1  ->  strong LPF (lots of HF damping)")
print(r"alpha = 0.5  ->  moderate LPF")
print(r"alpha = 0.95 ->  almost all-pass (no HF damping)")
print("\nDone: lpf_response.png")
