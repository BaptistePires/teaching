#!/bin/sh

gnuplot -e "
set terminal pdfcairo;
set output '$2';
set xlabel 'Cache line size (bytes)';
set ylabel 'Execution time (ns)';
set grid xtics;
plot '$1' using 1:2 w linespoints;
"
