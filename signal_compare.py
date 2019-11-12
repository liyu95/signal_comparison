#!/usr/bin/env python
from scipy import signal
import numpy as np
import sys
import os
from numpy import linalg as LA
from scipy import stats
import argparse

#raw = "001c577a-a502-43ef-926a-b883f94d157b.raw_signal"
#ds_old = 'sigout_e0.0_f-1_s0.0_B3.txt'
#ds_new  = 'sigout_e1.0_f850_s1.5_B3.txt'


#------- main function --------#
def main(arg):

	#-> open file and load
	sig_raw = np.loadtxt(arg.raw)
	sig_old = np.loadtxt(arg.old)
	sig_new = np.loadtxt(arg.new)

	#-> provide scale data structure
	scalar = np.arange(1, arg.s_range, arg.s_reso)

	#-> perform wavelet CWT
	cwtmatr_raw = signal.cwt(sig_raw, signal.ricker, scalar)
	cwtmatr_old = signal.cwt(sig_old, signal.ricker, scalar)
	cwtmatr_new = signal.cwt(sig_new, signal.ricker, scalar)

	# z score
	cwtmatr_raw = stats.zscore(cwtmatr_raw, axis=1)
	cwtmatr_old = stats.zscore(cwtmatr_old, axis=1)
	cwtmatr_new = stats.zscore(cwtmatr_new, axis=1)

	# calculate difference and correlation
	# print('Here is the L2 norm difference:')
	diff_old = LA.norm(cwtmatr_raw-cwtmatr_old)
	diff_new = LA.norm(cwtmatr_raw-cwtmatr_new)
	# print("Difference between raw and DS1.0: {}".format(diff_old))
	# print("Difference between raw and DS1.5: {}".format(diff_new))
	# print("The improvement ratio is {}%".format((diff_old-diff_new)/diff_old))

	# here is the correlation
	corr_old = np.corrcoef(cwtmatr_raw.flatten(), cwtmatr_old.flatten())[0, 1]
	corr_new = np.corrcoef(cwtmatr_raw.flatten(), cwtmatr_new.flatten())[0, 1]
	# print(np.corrcoef(cwtmatr_raw.flatten(), cwtmatr_old.flatten()))
	# print(np.corrcoef(cwtmatr_raw.flatten(), cwtmatr_new.flatten()))
	corr_old_row = list()
	corr_new_row = list()
	for i in range(10):
		corr_old_row.append(np.corrcoef(cwtmatr_raw[i, :], cwtmatr_old[i, :])[0, 1])
		corr_new_row.append(np.corrcoef(cwtmatr_raw[i, :], cwtmatr_new[i, :])[0, 1])
	corr_old_mean = np.mean(corr_old_row)
	corr_new_mean = np.mean(corr_new_row)
	if arg.d:
		stat = np.array([diff_old, diff_new, corr_old, corr_new, corr_old_mean, corr_new_mean])
		np.save(arg.d, stat)


#-------------- main -----------------#
if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Compare simulated signals from DS1.0, DS1.5 with raw signals.')
	parser.add_argument('-i', action='store', dest='raw', required=True, help='raw signal')
	parser.add_argument('-s', action='store', dest='old', required=True, help='simulated signal from DS1.0')
	parser.add_argument('-S', action='store', dest='new', required=True, help='simulated signal from DS1.5')
	parser.add_argument('-f', action='store', dest='s_range', type=float, help='scale range in CWT', default=40.0 )
	parser.add_argument('-r', action='store', dest='s_reso', type=float, help='scale resolution in CWT', default=0.1 )
	parser.add_argument('-d', action='store', dest='d', type=str, help='destination to save the statistics')
	arg = parser.parse_args()
	#-- main process --#
	main(arg)


