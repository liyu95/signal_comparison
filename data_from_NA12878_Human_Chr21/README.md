Here we put the data for comparing the simulated signals from DS1.5 and DS1.0 with the corresponding raw signals.
In doing so, we first randomly sampled 100 raw signals, and obtained the ground-truth reference sequence, using the NA12878_Human_Chr21 dataset.


# Part 1: signal simulation
We then simulate the signals by the folllwing two strategies:

## 1) simulated signals by DS1.0
```
deep_simulator.sh -i truth_fasta/<given_id>.fasta -n -1 -G 1 -e 0 -f 0 -s 0 -B 3 -o <given_id>_DS1.0
```

## 2) simulated signals by DS1.5
```
deep_simulator.sh -i truth_fasta/<given_id>.fasta -n -1 -G 1 -e 1.0 -f 850 -s 1.5 -B 3 -o <given_id>_DS1.5
```
### [note]: these simulated signals are generated and stored in 'signal_sim_DS1.0/' and 'signal_sim_DS1.5/', respectively.


# Part 2: re-warp simulated signals
After the simulated signals are generated, we "re-warp" them to the same length as the raw signal, which faciliated the downstream analysis.

To "re-warp" the 1st signal onto the 2nd signal (denoted as 'pivot' signal), please follow the below instructions:

## 1) align the two signals by cwDTW, and always put the pivot signal at second. Here we regard raw signal as pivot.
```
../cwDTW -i <simulated_signal> -p <raw_signal> -o alignment
```

## 2) re-warp the 1st signal onto the pivot signal according to the alignment.
```
../AlignedSignal_Proc <simulated_signal> <raw_signal> alignment 1 rewarp_signal 
awk '{print $1}' rewarp_signal > rewarp_signal.txt
```
### [note]: these re-warped simulated signals are stored in 'signal_sim_DS1.0_rewarp/' and 'signal_sim_DS1.5_rewarp/', respectively.



# Part 3: assess simulated signals
Finally, we may calculate the similatiry or perform some spectrum analysis of the simulated signals and the raw signals.

This may allow us to show the fact that the simulated signal by DS1.5 is "more similar" to the raw signal than that by DS1.0.

## 1) calculate the similarity
```
source activate simusig_comp
python ../signal_compare.py -i signal_raw/<given_id>.signal -s signal_sim_DS1.0_rewarp/<given_id>.signal -S signal_sim_DS1.5_rewarp/<given_id>.signal
source deactivate
```

## 2) plot the CWT spetrum of raw signal, simulated signal by DS1.0, and simulated signal by DS1.5
```
source activate simusig_comp
python ../plot_png.py -i signal_raw/<given_id>.signal -o raw -s 40000 -t 41000
python ../plot_png.py -i signal_sim_DS1.0_rewarp/<given_id>.signal -o ds10 -s 40000 -t 41000
python ../plot_png.py -i signal_sim_DS1.5_rewarp/<given_id>.signal -o ds15 -s 40000 -t 41000
source deactivate
```

### [note]: the CWT spetrum is saved to '_cwt.png' file.


