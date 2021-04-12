import matplotlib.pyplot as plt
import numpy as np
import csv

t = [] # column 0
s1 = [] # column 1

dt = 1.0/100.0 # 100Hz
with open('/home/pyu2020/Downloads/ME433_2021-main/HW2-DSP/sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        s1.append(float(row[1])) # second column


Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y =  s1 # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)

T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,color = 'black', label = 'old data')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),color = 'black', label = 'old data') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')

#add MAF filter
X = 400 # the last X number of data points
i = 1
M = []
A = s1
while i<len(A)-X+1:
    B = A[-(i+X):-i] # select the last X numbers
    B_ave = sum(B)/X # average the last X numbers
    M.append(B_ave) # collect the new number
    i +=1

N = M[::-1]
s = np.hstack(([0]*X, N))
ax1.plot(t,s,color = 'red', label = 'new data')

y_new =  s # the data to make the fft from
n_new = len(y_new) # length of the signal
k_new = np.arange(n_new)

T_new = n_new/Fs
frq_new = k_new/T # two sides frequency range
frq_new = frq_new[range(int(n_new/2))] # one side frequency range
Y_new = np.fft.fft(y_new)/n_new # fft computing and normalization
Y_new = Y_new[range(int(n/2))]
ax2.loglog(frq,abs(Y_new),color = 'red', label = 'new data') # plotting the fft
ax1.set_title('The number of data points averaged is 400')
plt.show()

# write csv file
file = open("sig_average_data.csv", "w")
writer = csv.writer(file)
for w in range(len(s)):
    writer.writerow([t[w],s[w]])
file.close()
