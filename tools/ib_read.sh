#!/bin/bash
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2020, Intel Corporation
#

#
# ib_read.sh -- a single-sided ib_read_lat/bw tool (EXPERIMENTAL)
#
# Spawns both server and the client, collects the results for multile data
# sizes (1KiB, 4KiB, 64KiB) and generates a single CSV file with all
# the resutls.
#

HEADER_LAT="#bytes #iterations    t_min[usec]    t_max[usec]  t_typical[usec]    t_avg[usec]    t_stdev[usec]   99% percentile[usec]   99.9% percentile[usec]"
HEADER_BW="#threads #bytes     #iterations    BW_peak[Gb/sec]    BW_average[Gb/sec]   MsgRate[Mpps]"

TIMESTAMP=$(date +%y-%m-%d-%H%M%S)

echo "This tool is EXPERIMENTAL"

function usage()
{
	echo "Error: $1"
	echo
	echo "usage: $0 <bw-bs|bw-dp|bw-th|lat> <server_ip>"
	echo
	echo "export JOB_NUMA=0"
	echo "export AUX_PARAMS='-d mlx5_0 -R'"
	echo "export REMOTE_USER=user"
	echo "export REMOTE_PASS=pass"
	echo "export REMOTE_JOB_NUMA=0"
	echo "export REMOTE_AUX_PARAMS='-d mlx5_0 -R'"
	exit 1
}

if [ "$#" -lt 2 ]; then
	usage "Too few arguments"
elif [ -z "$JOB_NUMA" ]; then
	usage "JOB_NUMA not set"
elif [ -z "$REMOTE_USER" ]; then
	usage "REMOTE_USER not set"
elif [ -z "$REMOTE_PASS" ]; then
	usage "REMOTE_PASS not set"
elif [ -z "$REMOTE_JOB_NUMA" ]; then
	usage "REMOTE_JOB_NUMA not set"
fi

MODE=$1
SERVER_IP=$2

function verify_block_size()
{
	if [ ${#BLOCK_SIZE[@]} -ne ${#ITERATIONS[@]} ]; then
		echo "Error: sizes of the arrays: BLOCK_SIZE(${#BLOCK_SIZE[@]}) and ITERATIONS(${#ITERATIONS[@]}) are different!"
		exit 1
	fi
}

function verify_threads()
{
	if [ ${#THREADS[@]} -ne ${#ITERATIONS[@]} ]; then
		echo "Error: sizes of the arrays: THREADS(${#THREADS[@]}) and ITERATIONS(${#ITERATIONS[@]}) are different!"
		exit 1
	fi
}

function verify_depth()
{
	if [ ${#DEPTH[@]} -ne ${#ITERATIONS[@]} ]; then
		echo "Error: sizes of the arrays: DEPTH(${#DEPTH[@]}) and ITERATIONS(${#ITERATIONS[@]}) are different!"
		exit 1
	fi
}

case $MODE in
bw-bs)
	IB_TOOL=ib_read_bw
	HEADER=$HEADER_BW
	THREADS=1
	BLOCK_SIZE=(256 1024 4096 8192 65536)
	DEPTH=2
	# values measured empirically, so that duration was ~60s
	# 100000000 is the maximum value of iterations
	ITERATIONS=(25831474 23448108 15927940 12618268 5001135)
	AUX_PARAMS="$AUX_PARAMS --report_gbits"
	NAME="${MODE}-${THREADS}th"
	verify_block_size
	;;
bw-dp)
	IB_TOOL=ib_read_bw
	HEADER=$HEADER_BW
	THREADS=1
	BLOCK_SIZE=4096
	DEPTH=(1 2 4 8 16 32 64 128)
	# values measured empirically, so that duration was ~60s
	# 100000000 is the maximum value of iterations
	ITERATIONS=(20769620 30431214 45416656 65543498 85589536 100000000 100000000 100000000)
	AUX_PARAMS="$AUX_PARAMS --report_gbits"
	NAME="${MODE}-${BLOCK_SIZE}bs"
	verify_depth
	;;
bw-th)
	IB_TOOL=ib_read_bw
	HEADER=$HEADER_BW
	# XXX TH=16 hangs the ib_read_bw at the moment
	# XXX TH=16 takes 11143637 iterations to run for ~60s
	THREADS=(1 2 4 8 12)
	BLOCK_SIZE=4096
	DEPTH=2
	# values measured empirically, so that duration was ~60s
	# 100000000 is the maximum value of iterations
	ITERATIONS=(16527218 32344690 61246542 89456698 89591370)
	AUX_PARAMS="$AUX_PARAMS --report_gbits"
	NAME="${MODE}-${BLOCK_SIZE}bs"
	verify_threads
	;;
lat)
	IB_TOOL=ib_read_lat
	HEADER=$HEADER_LAT
	THREADS=1
	BLOCK_SIZE=(1024 4096 65536)
	DEPTH=1
	# values measured empirically, so that duration was ~60s
	ITERATIONS=(27678723 20255739 6002473)
	AUX_PARAMS="$AUX_PARAMS --perform_warm_up"
	NAME="${MODE}"
	verify_block_size
	;;
*)
	usage "Wrong mode: $MODE"
	;;
esac

OUTPUT=ib_read_${NAME}-${TIMESTAMP}.csv
LOG_ERR=/dev/shm/ib_read_${NAME}-err-${TIMESTAMP}.log

echo "Performance results: $OUTPUT"
echo "Output and errors (both sides): $LOG_ERR"
echo

rm -f $LOG_ERR
echo "$HEADER" | sed 's/% /%_/g' | sed -r 's/[[:blank:]]+/,/g' > $OUTPUT

for i in $(seq 0 $(expr ${#ITERATIONS[@]} - 1)); do
	case $MODE in
	bw-bs)
		IT=${ITERATIONS[${i}]}
		BS="${BLOCK_SIZE[${i}]}"
		TH="${THREADS}"
		DP="${DEPTH}"
		IT_OPT="--iters $IT"
		BS_OPT="--size $BS"
		QP_OPT="--qp $TH"
		DP_OPT="--tx-depth=${DP}"
		echo -n "${TH},${DP}," >> $OUTPUT
		;;
	bw-dp)
		IT=${ITERATIONS[${i}]}
		BS="${BLOCK_SIZE}"
		TH="${THREADS}"
		DP="${DEPTH[${i}]}"
		IT_OPT="--iters $IT"
		BS_OPT="--size $BS"
		QP_OPT="--qp $TH"
		DP_OPT="--tx-depth=${DP}"
		echo -n "${TH},${DP}," >> $OUTPUT
		;;
	bw-th)
		IT=${ITERATIONS[${i}]}
		BS="${BLOCK_SIZE}"
		TH="${THREADS[${i}]}"
		DP="${DEPTH}"
		IT_OPT="--iters $IT"
		BS_OPT="--size $BS"
		QP_OPT="--qp $TH"
		DP_OPT="--tx-depth=${DP}"
		echo -n "${TH},${DP}," >> $OUTPUT
		;;
	lat)
		IT=${ITERATIONS[${i}]}
		BS="${BLOCK_SIZE[${i}]}"
		TH="$THREADS"
		DP="${DEPTH}"
		IT_OPT="--iters $IT"
		BS_OPT="--size $BS"
		QP_OPT=""
		DP_OPT=""
		;;
	esac

	# run the server
	sshpass -p "$REMOTE_PASS" -v ssh $REMOTE_USER@$SERVER_IP \
		"numactl -N $REMOTE_JOB_NUMA ${IB_TOOL} $BS_OPT $QP_OPT $DP_OPT \
		$REMOTE_AUX_PARAMS >> $LOG_ERR" 2>>$LOG_ERR &
	sleep 1

	# XXX --duration hides detailed statistics
	echo "[size: ${BS}, threads: ${TH}, tx_depth: ${DP}, iters: ${IT}] (duration: ~60s)"
	numactl -N $JOB_NUMA ${IB_TOOL} $IT_OPT $BS_OPT $QP_OPT $DP_OPT $AUX_PARAMS $SERVER_IP 2>>$LOG_ERR \
		| grep ${BS} | grep -v '[B]' | sed 's/^[ ]*//' \
		| sed 's/[ ]*$//' | sed -r 's/[[:blank:]]+/,/g' >> $OUTPUT
done

CSV_MODE=$(echo ${IB_TOOL} | sed 's/_read//')

# convert to standardized-CSV
./csv2standardized.py --csv_type ${CSV_MODE} --output_file $OUTPUT $OUTPUT
