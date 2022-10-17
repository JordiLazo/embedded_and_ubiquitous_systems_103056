import os
import random
from itertools import count
import datetime as dt
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
"""
plt.ion()
x_vals = []
y_vals = []

plt.plot([], [], label='Temperatura')
plt.plot([], [], label='Humitat')


def animate(i):
    data = pd.read_csv('base_dades_tfg.csv',names=['timestamp', 'id', 'temperature', 'humidity'])
    x = [dt.datetime.fromtimestamp(int(ts)) for ts in data['timestamp']]
    y1 = data['temperature']
    y2 = data['humidity']

"""
plt.style.use('fivethirtyeight')

x_vals = []
y_vals = []

index = count()


def animate(i):
    data = pd.read_csv('base_dades_didac.csv',names=['timestamp', 'id', 'heart', 'accelerometer'])
    x = [dt.datetime.fromtimestamp(int(ts)) for ts in data['timestamp']]
    y1 = data['heart']
    y2 = data['accelerometer']

    plt.cla()

    plt.plot(x, y1, label='PPM')
    plt.plot(x, y2, label='Acelerometro')

    plt.legend(loc='upper left')
    plt.tight_layout()


ani = FuncAnimation(plt.gcf(), animate, interval=100)

plt.tight_layout()
plt.show()