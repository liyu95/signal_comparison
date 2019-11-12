import matplotlib
matplotlib.use('agg')
import pylab as plt
import numpy as np # linear algebra
import pandas as pd # data processing, CSV file I/O (e.g. pd.read_csv)
import seaborn as sns
import scipy.fftpack
import argparse

# currently (03/2019) scaleogram needs install via https://github.com/alsauve/scaleogram
# because an old PyWavelets (0.5.2) is installend and PyWavelets > 1.0 is needed
import scaleogram as scg 
from scipy import signal
import pywt



#------- main function --------#
def main(arg):

	# load the raw signal
	sig_ori = np.loadtxt(arg.input)
	if arg.termi == -1:
		sig = sig_ori[arg.start:]
	else:
		sig = sig_ori[arg.start:arg.termi]
	ns = len(sig)
	time = np.arange(ns)

	# and a range of scales to perform the transform
	scales = scg.periods2scales( np.arange(1, arg.s_range, arg.s_reso) )

	# plot the signal 
	fig1, ax1 = plt.subplots(1, 1, figsize=(6.2,2));  
	lines = ax1.plot(sig); 
	ax1.set_xlim(0, len(time))
	ax1.set_title('raw signal from '+str(arg.start)+' to '+str(arg.termi))
	fig1.tight_layout()
	plt.savefig(arg.output+'_sig')

	# and the scaleogram
	ax2 = scg.cws(sig, scales=scales, figsize=(6.9,6.3)); 
	#txt = ax2.annotate("p1=10s", xy=(100, 10), bbox=dict(boxstyle="round4", fc="w"))
	plt.tight_layout()
	plt.savefig(arg.output+'_cwt')
	#print("Default wavelet function used to compute the transform:", scg.get_default_wavelet(), "(",
	#      pywt.ContinuousWavelet(scg.get_default_wavelet()).family_name, ")")

	# calculate CWT matrix
	cwtmatr = signal.cwt(sig, signal.ricker, scales)
	out = np.transpose(cwtmatr)
	np.savetxt(arg.output+'.mat', out)


	# plot FFT figure
	# sample spacing
	T = 1.0 / 4000.0
	N = ns
	yf = scipy.fftpack.fft(sig)
	xf = np.linspace(0.0, 1.0/(2.0*T), N/2)
	fig, ax = plt.subplots()
	ax.plot(xf, 2.0/N * np.abs(yf[:N//2]))
	plt.ylim(0,50)
	plt.savefig(arg.output+'_fft')


#-------------- main -----------------#
if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Plot continuous wavelet transform in a given range of the input signal.')
	parser.add_argument('-i', action='store', dest='input', required=True, help='the file of the input signal')
	parser.add_argument('-o', action='store', dest='output', required=True, help='the output name for png files')
	parser.add_argument('-s', action='store', dest='start', type=int, help='start position (0-base)', default=0 )
	parser.add_argument('-t', action='store', dest='termi', type=int, help='termi position (0-base)', default=-1 )
	parser.add_argument('-f', action='store', dest='s_range', type=float, help='scale range in CWT', default=40.0 )
	parser.add_argument('-r', action='store', dest='s_reso', type=float, help='scale resolution in CWT', default=0.1 )
	arg = parser.parse_args()
	#-- main process --#
	main(arg)

