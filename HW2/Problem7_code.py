import matplotlib.pyplot as plt
import numpy as np
import csv

H = [
    -0.000000000000000001,
    0.000217873144985824,
    0.000902282820411401,
    0.002139390798131369,
    0.004053011915037439,
    0.006777986176793080,
    0.010428575014102966,
    0.015066606029803184,
    0.020674227751655248,
    0.027135597461970515,
    0.034230680964823006,
    0.041642745586689678,
    0.048979291245368470,
    0.055804336650377741,
    0.061678405905247209,
    0.066201458549007747,
    0.069053524383354326,
    0.070028011204481808,
    0.069053524383354326,
    0.066201458549007774,
    0.061678405905247223,
    0.055804336650377720,
    0.048979291245368511,
    0.041642745586689713,
    0.034230680964823013,
    0.027135597461970515,
    0.020674227751655231,
    0.015066606029803191,
    0.010428575014102982,
    0.006777986176793085,
    0.004053011915037445,
    0.002139390798131376,
    0.000902282820411401,
    0.000217873144985825,
    -0.000000000000000001,
]
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

#add combination of the MAF and IIR
X = len(H) # the last X number of data points
i = 1
M = []
A = s1
while i<len(A)-X+1:
    B = A[-(i+X):-i] # select the last X numbers
    sum_B = np.multiply(B,H)
    B_ave = sum(sum_B) # average the last X numbers
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
ax1.set_title("The number of data points averaged is %s"%X)
plt.show()

# write csv file
file = open("sig_average_data.csv", "w")
writer = csv.writer(file)
for w in range(len(s)):
    writer.writerow([t[w],s[w]])
file.close()