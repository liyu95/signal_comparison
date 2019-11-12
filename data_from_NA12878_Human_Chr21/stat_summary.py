import numpy as np
import os

folder = 'stat_z_scored'

files = os.listdir(folder)

results = list()

for file in files:
	tmp = np.load(os.path.join(folder, file))
	results.append(tmp)

results = np.array(results)

print('Here is the average improvement ratio:')
print((np.mean(results[:, 0])-np.mean(results[:, 1]))/np.mean(results[:, 0]))