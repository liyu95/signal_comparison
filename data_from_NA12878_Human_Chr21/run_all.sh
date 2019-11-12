source activate simusig_comp


for i in `ls signal_raw`; do
	#statements
	python ../signal_compare.py -i signal_raw/${i} \
								-s signal_sim_DS1.0_rewarp/${i} \
								-S signal_sim_DS1.5_rewarp/${i} \
								-d stat_z_scored/${i%.*}.npy &
done

