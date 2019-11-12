# README

# For install
Configure necessary environment. Only for the first time:
```
./install.sh
```

# align two signals, and we ALWAYS put the raw signal in second as pivot
./cwDTW -i example/signal_preprocess.txt -p example/001c577a-a502-43ef-926a-b883f94d157b.raw_signal -o alignment

# we transform simu to raw in length
./AlignedSignal_Proc example/signal_preprocess.txt example/001c577a-a502-43ef-926a-b883f94d157b.raw_signal alignment 1 sigout
awk '{print $1}' sigout > sigout.txt

# plot PNG files
source activate simusig_comp
python plot_png.py -i sigout.txt -o sig -s 40000 -t 41000
source deactivate

# compare simulated from DS1.0 and DS1.5 with the raw signal
source activate simusig_comp
python signal_compare.py -i example/001c577a-a502-43ef-926a-b883f94d157b.raw_signal -s example/signal_preprocess_e0.0_f-1_s0.0_B3.simu_signal_rewarp -S example/signal_preprocess_e1.0_f850_s1.5_B3.simu_signal_rewarp -d example/signal_preprocess_e1.0_f850_s1.5_B3.simu_signal_rewarp.npy
source deactivate

