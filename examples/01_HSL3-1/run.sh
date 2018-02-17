#! /bin/bash
#                               -*- Mode: Sh -*- 
# HSL3-1.sh --- runs experiment HSL3 1
# Author          : Marcel Turcotte
# Created On      : Thu Aug  4 14:19:39 2005
# Last Modified By: Marcel Turcotte
# Last Modified On: Fri Sep  2 09:01:23 2005
# 

DATA="data.fas"

[ -d results ] && rm -rf results

mkdir results

SEED_PARAMS=
SEED_PARAMS="$SEED_PARAMS --stem_min_len 6"
SEED_PARAMS="$SEED_PARAMS --max_num_stem 1"
SEED_PARAMS="$SEED_PARAMS --stem_max_separation 30"
SEED_PARAMS="$SEED_PARAMS --range 0"
SEED_PARAMS="$SEED_PARAMS --max_mismatch 0"
SEED_PARAMS="$SEED_PARAMS --destination results"
SEED_PARAMS="$SEED_PARAMS --match_file results/matches.xml"
SEED_PARAMS="$SEED_PARAMS --save_all_matches"
# SEED_PARAMS="$SEED_PARAMS --save_as_ct"
SEED_PARAMS="$SEED_PARAMS $DATA"

/Users/turcotte/Sync/Lab/Seed-1.0/seed/src/algorithms/seed $SEED_PARAMS > results/run.out
