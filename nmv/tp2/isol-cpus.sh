#!/usr/bin/env bash
set -e

#############################################
# CONFIGURATION
#############################################

# >>> CHANGE THIS to the physical core you want to isolate <<<
ISOLATED_CORE_ID=5      # example: physical core #5 (will isolate its two SMT threads)

#############################################
# Detect CPU topology
#############################################

echo "[+] Detecting SMT siblings for physical core $ISOLATED_CORE_ID"

# Find CPUs belonging to that core
CPUS=$(lscpu -e=CPU,CORE | awk -v c=$ISOLATED_CORE_ID '$2==c {print $1}' | tr '\n' ',' | sed 's/,$//')

if [[ -z "$CPUS" ]]; then
    echo "ERROR: Could not find CPUs for core $ISOLATED_CORE_ID"
    exit 1
fi

echo "[+] SMT threads for core $ISOLATED_CORE_ID: $CPUS"

#############################################
# Determine housekeeping CPUs
#############################################

TOTAL_CPUS=$(nproc --all)
ALL_CPUS=$(seq -s , 0 $((TOTAL_CPUS-1)))

# Remove isolated CPUs from the list
HOUSEKEEPING_CPUS=$(echo $ALL_CPUS | sed "s/\b$CPUS\b//g" | sed 's/,,*/,/g' | sed 's/^,//' | sed 's/,$//')

echo "[+] Housekeeping CPUs: $HOUSEKEEPING_CPUS"

#############################################
# Prepare cgroup v2 cpuset layout
#############################################

cd /sys/fs/cgroup

echo "[+] Enabling cpuset controller"
echo "+cpuset" > cgroup.subtree_control 2>/dev/null || true

echo "[+] Creating cgroups"
mkdir -p housekeeping isolated

#############################################
# Configure cpusets
#############################################

echo "[+] Configuring housekeeping cpuset"
echo $HOUSEKEEPING_CPUS > housekeeping/cpuset.cpus
echo 0 > housekeeping/cpuset.mems

echo "[+] Configuring isolated cpuset"
echo $CPUS > isolated/cpuset.cpus
echo 0 > isolated/cpuset.mems

#############################################
# Move system tasks to housekeeping
#############################################

echo "[+] Moving tasks into housekeeping"
while read -r pid; do
    echo $pid > housekeeping/cgroup.procs 2>/dev/null || true
done < cgroup.procs

#############################################
# Optional: move IRQs off the isolated core
#############################################

echo "[+] Stopping irqbalance (if running)"
systemctl stop irqbalance 2>/dev/null || true

echo "[+] Moving IRQs away from isolated CPUs: $CPUS"
for irq in $(ls /proc/irq | grep '^[0-9]\+$'); do
    echo $HOUSEKEEPING_CPUS > /proc/irq/$irq/smp_affinity_list 2>/dev/null || true
done

#############################################
# Done
#############################################

echo ""
echo "============================================="
echo "  SUCCESS — SMT core isolated"
echo "---------------------------------------------"
echo "  Isolated CPUs:      $CPUS"
echo "  Housekeeping CPUs:  $HOUSEKEEPING_CPUS"
echo "---------------------------------------------"
echo "To run a workload on the isolated core:"
echo "    taskset -c $CPUS ./your_program"
echo ""
echo "To place a PID in isolated cpuset:"
echo "    echo <PID> > /sys/fs/cgroup/isolated/cgroup.procs"
echo "============================================="
