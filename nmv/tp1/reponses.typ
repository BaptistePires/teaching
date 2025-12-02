= NMV - TP1 - Gestion de la mémoire virtuelle
== Exercice 1 
=== Question 1
On a besoin qu'ils soient mappés par identité parce que ce sont les adresses des niveaux suivant qui sont stockés, pour y accèder, on ne va pas repasser par la la mmu et redemander une traduction d'adresse, il faut que ce soit des adressses par identités sinon on ne peut pas parcourir la table des pages.

=== Question 2
On a :
- 12 bits pour les flags
- 9 bits par niveaux -> 9 x 4 = 36 bits
- 12 bits pour l'extension de signe

Pour chaque niveau, nous avons donc $2^9 = 512$ entrées.

=== Question 3
```c

  void print_pgt(paddr_t pml, uint8_t level)
  {
    paddr_t *current_pml = (paddr_t *)pml;
    paddr_t pte_entry, next_pml_addr;

    if (level == 0)
      return;

    /* Pour toutes les entrees de pmlX */
    for (uint16_t i = 0; i < PGT_NR_ENTRIES; i++) {
      
      /* Entree courante */
      pte_entry = current_pml[i];
      
      /* Si l'entrée dans la pgt n'est pas valide on continue */
      if (!PTE_IS_VALID(pte_entry))
        continue;

      /* On recupere l'@ de la next */
      next_pml_addr = PTE_NEXT_ADDR(pte_entry);

      for (uint8_t l = level; l < 4 ; l++)
        printk("\t");

      printk("level=%u, index=%u, flags=%3lx, next_addr=%p, hugepage=%u\n", level, i, pte_entry & 0xfff, next_pml_addr, !!PTE_IS_HUGE(pte_entry));
      
      if (!PTE_IS_HUGE(pte_entry))
        print_pgt(next_pml_addr, level - 1);
    }
  }
```