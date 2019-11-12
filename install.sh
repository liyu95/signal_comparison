#!/bin/bash


#-> 1. install tensorflow_cdpm
conda remove --name simusig_comp --all -y
conda create --name simusig_comp python=2.7 -y
source activate simusig_comp
pip install scaleogram
pip install pandas
pip install seaborn
source deactivate

