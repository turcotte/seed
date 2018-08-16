#! /bin/bash
#                               -*- Mode: Sh -*- 
# run.sh --- runs IRE 2 experiment
# Author          : Marcel Turcotte
# Created On      : Thu Aug  4 14:19:39 2005
# Last Modified By: turcotte
# Last Modified On: Thu Aug 16 08:14:54 2018
# 

DATA="data.fas"

[ -d results ] && rm -rf results

mkdir results

SEED_PARAMS=
SEED_PARAMS="$SEED_PARAMS --stem_min_len 3"
SEED_PARAMS="$SEED_PARAMS --stem_max_separation 30"
SEED_PARAMS="$SEED_PARAMS --range 0"
SEED_PARAMS="$SEED_PARAMS --max_mismatch 0"
SEED_PARAMS="$SEED_PARAMS --destination results"
SEED_PARAMS="$SEED_PARAMS --match_file results/matches.xml"
SEED_PARAMS="$SEED_PARAMS --save_all_matches"
# SEED_PARAMS="$SEED_PARAMS --save_as_ct"
SEED_PARAMS="$SEED_PARAMS $DATA"

../../bin/seed $SEED_PARAMS > results/run.out
