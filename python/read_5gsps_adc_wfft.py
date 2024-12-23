
import struct
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import cothread
from cothread.catools import *


MAXLEN = 12000 
Frf = 499.68e6
Fin = 499.68e6
h = 1320
hADC = 310*40
hIf = 1320
Fs = Frf/h*hADC
Ftbt = Frf/h
Ts =  1/Fs
adcFS = 8192.0


def get_adc_data(bpm_prefix):

  #create pv list
  adc_pv = []
  adc_pv.append(bpm_prefix+"ADC:Live:ARaw-I")
  adc_pv.append(bpm_prefix+"ADC:Live:BRaw-I")
  adc_pv.append(bpm_prefix+"ADC:Live:CRaw-I")
  adc_pv.append(bpm_prefix+"ADC:Live:DRaw-I")

  adc_data = caget(adc_pv)
  #for i in range(10): 
  #  print(adc_data[0][i],adc_data[1][i],adc_data[2][i],adc_data[3][i])

  return adc_data



 


def main():

    plt.ion()
    adc_data = get_adc_data("LN-BI{BPM:1}")
    
    
    figure, axes = plt.subplots(nrows=4, ncols=1, figsize=(8,4))
    line1, = axes[0].plot(adc_data[0], marker='o', color='b') 
    line2, = axes[1].plot(adc_data[1])
    line3, = axes[2].plot(adc_data[2]) 
    line4, = axes[3].plot(adc_data[3])    
    axes[0].set_title('ADC Data')
    axes[0].set_xlabel('Sample #')
    axes[0].set_ylabel('adu')
    axes[0].set_ylim(-1*np.max(adc_data[0])-25,np.max(adc_data[0])+25) 
    axes[0].grid(True)
    axes[1].set_title('ADC Data')
    axes[1].set_xlabel('Sample #')
    axes[1].set_ylabel('adu')
    axes[1].set_ylim(-1*np.max(adc_data[1])-25,np.max(adc_data[1])+25)
    axes[1].grid(True)
    axes[2].set_title('ADC Data')
    axes[2].set_xlabel('Sample #')
    axes[2].set_ylabel('adu')
    axes[2].set_ylim(-1*np.max(adc_data[2])-25,np.max(adc_data[2])+25)
    axes[2].grid(True)
    axes[3].set_title('ADC Data')
    axes[3].set_xlabel('Sample #')
    axes[3].set_ylabel('adu')
    axes[3].set_ylim(-1*np.max(adc_data[3])-25,np.max(adc_data[3])+25)
    axes[3].grid(True) 
    



    try:
      while True:
        adc_data = get_adc_data("LN-BI{BPM:1}")
        print("Plotting...")
        line1.set_ydata(adc_data[0])
        line2.set_ydata(adc_data[1])
        line3.set_ydata(adc_data[2])
        line4.set_ydata(adc_data[3])

        figure.canvas.draw() 
        figure.canvas.flush_events()
        time.sleep(0.1) 
        time.sleep(1)
    except KeyboardInterrupt:
        print("Plotting stopped")
    #input('Press any key to quit...')
    


'''
    adcA = np.zeros(MAXLEN,dtype=np.int16)
    adcB = np.zeros(MAXLEN,dtype=np.int16)
    adcC = np.zeros(MAXLEN,dtype=np.int16)
    adcD = np.zeros(MAXLEN,dtype=np.int16)
    x = np.linspace(0,MAXLEN-1,MAXLEN)

    plt.ion()

    figure, axes = plt.subplots(nrows=2, ncols=1, figsize=(8,4))
    line1, = axes[0].plot(x,adcA) 
    line2, = axes[1].plot(x,adcA)
    axes[0].set_title('ADC Data')
    axes[0].set_xlabel('Sample #')
    axes[0].set_ylabel('adu')
    axes[0].set_ylim(-8192,8192) # 14-bit ADC range
    axes[0].grid(True)
    axes[1].set_xlabel('Freq')
    axes[1].set_ylabel('Power')
    axes[1].set_ylim(-160,10) # 14-bit ADC range
    axes[1].set_xlim(0,Fs/2/1e6)
    axes[1].grid(True)



    while True:


        adcA = get_adcdata(m,adcA)
        y,p = calc_psd(adcA)


        print("Plotting...")
        line1.set_xdata(x)
        line1.set_ydata(adcA)

        n = len(p)*2-1
        print("Fs = ",Fs)
        f = np.linspace(0,Fs/2,n/2+1)
        f = f/1e6
        
        print("n = ",n)
        print("length f = ",len(f))
        print("length p = ",len(p))
        print("length y = ",len(y))

        line2.set_xdata(f)
        line2.set_ydata(p)
        
        figure.canvas.draw() 
        figure.canvas.flush_events()
        time.sleep(0.1) 

    #input('Press any key to quit...')
    m.close()
    os.close(f)
'''


if __name__ == "__main__":
    main()



