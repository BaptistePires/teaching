#!/bin/sh

gnuplot -e "
set terminal pdfcairo;
set output '$2';
set grid xtics;
set xtics rotate by -25 offset -2,0 font ',10';
plot '$1' using 1:2 w linespoints;
"

# set yrange [0:11000];
# set yrange [0:223600];

# set logscale x;
# set logscale x 2;

