#!/bin/bash

nohup SKFlat.py -a Vcb_Mu -i SingleMuon -n 100 -e 2018 &
nohup SKFlat.py -a Vcb_Mu -l Input.txt -n 100 -e 2018 &