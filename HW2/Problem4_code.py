import matplotlib.pyplot as plt
import numpy as np
import csv

t = [] # column 0
s = [] # column 1

dt = 1.0/100.0 # 100Hz
with open('/home/pyu2020/Downloads/ME433_2021-main/HW2-DSP/sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        s.append(float(row[1])) # second column


Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
Ts = np.arange(0,t[-1],Ts) # time vector
y =  s # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()