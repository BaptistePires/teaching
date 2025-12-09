#!/bin/sh

gnuplot -e "
set terminal pdfcairo;
set output '$2';
set logscale x 2;
set grid xtics;
set yrange [-5:100];
set ytics 10;
set grid ytics;
set xlabel 'Cache line size (bytes)';
set ylabel 'Hit Rate (%)';
plot '$1' using 1:2 w linespoints;
"
