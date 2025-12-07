// #import "imports.typ": *
#import "functions.typ": *
#import "style.typ": *

// Page de garde
#align(center)[
  #v(4cm)
  #text(size: 23pt)[TP2 - Détection de la topologie mémoire]
  #v(1cm)
  #text(size: 12pt)[SAR M2 - Noyaux Multicoeurs et Virtualisation]
  #v(1cm)
  #place(bottom, float:true)[#text(size: 12pt)[Baptiste Pires - 03/12/2025]]
]


#pagebreak()



= Exercice 1
== Question 1
Comme Linux fait de la lazy allocation, même si on a fait malloc, la mémoire n'est pas réellement allouée.


== Question 2
Ca dépend de la valeur de #ccode("PARAM"). Si $"PARAM" < ("sizeof(mem[0])/cache_line_size")$