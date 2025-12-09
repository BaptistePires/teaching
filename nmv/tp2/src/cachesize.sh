#!/bin/sh

gnuplot -e "
set terminal pdfcairo;
set output '$2';
set logscale y;
set logscale x;
set logscale x 2;
set grid xtics;
set xtics rotate by -90;
set xtics (2048, 4096, 8192, 16384, 32768, 48576, 65536, 131072, 262144, 524288, 1310720, 12597152);
set format x \"%.0f\";
plot '$1' using 1:2 w linespoints;
"
