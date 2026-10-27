#!/bin/bash
# ----------------------------------------------------------------------------
# run_param_scan.sh
#
# Run the photon-only light-map simulation (macros/NEXTHDDEMO.*.mac) once per
# value of a single macro command, e.g. the cathode transparency or the
# fiber-sensor gap. For every value a dedicated init/config macro pair is
# written next to its output file, so each run is fully reproducible.
#
# Usage:
#   scripts/run_param_scan.sh -c <macro command> [-u <unit>] [-o <out dir>]
#                             [-n <events>] [-p <photons/event>] [-s <seed>]
#                             [-j <parallel jobs>] value1 [value2 ...]
#
# Examples:
#   scripts/run_param_scan.sh -c /Geometry/NextHDDEMO/cathode_transparency \
#       -o scans/cathode_transparency -n 2000 0.25 0.5 0.75 0.95 1.0
#
#   scripts/run_param_scan.sh -c /Geometry/NextHDDEMO/sensor_gap -u mm \
#       -o scans/sensor_gap -n 2000 0 0.05 0.1 0.25 0.5
#
# Output: <out dir>/<tag>_<value>/NextHDDEMO_<tag>_<value>.h5
# ----------------------------------------------------------------------------

set -euo pipefail

NEXUS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

CMD=""
UNIT=""
OUT_DIR=""
N_EVENTS=1000
N_PHOTONS=1000
SEED=12345
JOBS=1

while getopts "c:u:o:n:p:s:j:" opt; do
  case $opt in
    c) CMD="$OPTARG" ;;
    u) UNIT="$OPTARG" ;;
    o) OUT_DIR="$OPTARG" ;;
    n) N_EVENTS="$OPTARG" ;;
    p) N_PHOTONS="$OPTARG" ;;
    s) SEED="$OPTARG" ;;
    j) JOBS="$OPTARG" ;;
    *) sed -n '2,22p' "$0"; exit 1 ;;
  esac
done
shift $((OPTIND - 1))

if [[ -z "$CMD" || $# -eq 0 ]]; then
  sed -n '2,22p' "$0"
  exit 1
fi

TAG="$(basename "$CMD")"
OUT_DIR="$(realpath -m "${OUT_DIR:-$NEXUS_DIR/scans/$TAG}")"
mkdir -p "$OUT_DIR"

# setup.sh is not written for nounset/errexit
set +eu
source "$NEXUS_DIR/setup.sh" > /dev/null 2>&1
set -eu

run_point() {
  local value="$1"
  local run_dir="$OUT_DIR/${TAG}_${value}"
  local base="$run_dir/NextHDDEMO_${TAG}_${value}"
  mkdir -p "$run_dir"

  # Config: base photon-only config + the scanned command, own output file,
  # fixed photon number and seed (same seed for every point).
  sed -e "s|^/nexus/persistency/output_file.*|/nexus/persistency/output_file $base|" \
      -e "s|^/Generator/ScintGenerator/nphotons.*|/Generator/ScintGenerator/nphotons $N_PHOTONS|" \
      "$NEXUS_DIR/macros/NEXTHDDEMO.config.mac" > "$run_dir/scan.config.mac"
  {
    echo ""
    echo "##### PARAMETER SCAN #####"
    echo "$CMD $value $UNIT"
    echo "/nexus/random_seed $SEED"
  } >> "$run_dir/scan.config.mac"

  sed -e "s|^/nexus/RegisterMacro .*|/nexus/RegisterMacro $run_dir/scan.config.mac|" \
      "$NEXUS_DIR/macros/NEXTHDDEMO.init.mac" > "$run_dir/scan.init.mac"

  echo "[scan] $CMD = $value $UNIT -> $base.h5"
  (cd "$NEXUS_DIR" && ./bin/nexus -b -n "$N_EVENTS" "$run_dir/scan.init.mac") \
    > "$run_dir/nexus.log" 2>&1 \
    && echo "[scan] done: $value" \
    || echo "[scan] FAILED: $value (see $run_dir/nexus.log)"
}

for value in "$@"; do
  run_point "$value" &
  while (( $(jobs -rp | wc -l) >= JOBS )); do
    wait -n
  done
done
wait
