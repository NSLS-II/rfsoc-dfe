import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Sampling parameters
Fs = 1000  # Sampling frequency in Hz
t = np.linspace(0, 1, Fs)  # Time vector for 1 second

# Initial data: sine wave
data = np.sin(2 * np.pi * 50 * t)  # 50 Hz sine wave

# Set up the plot
fig, ax = plt.subplots()
line, = ax.plot(t, data)
ax.set_ylim(-1.1, 1.1)
ax.set_xlabel('Time [s]')
ax.set_ylabel('Amplitude')
ax.set_title('Real-Time Waveform')

# Update function
def update(frame):
    # Generate new data (e.g., sine wave with increasing frequency)
    data = np.sin(2 * np.pi * (50 + frame) * t)
    line.set_ydata(data)
    return line,

# Create the animation
ani = FuncAnimation(fig, update, frames=100, interval=1000, blit=True)

# Display the plot
plt.show()

