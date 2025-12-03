// #import "imports.typ": *
#import "functions.typ": *
#import "style.typ": *

// Page de garde
#align(center)[
  #v(4cm)
  #text(size: 23pt)[TP1 - Gestion de la mémoire virtuelle]
  #v(1cm)
  #text(size: 12pt)[SAR M2 - Noyaux Multicoeurs et Virtualisation]
  #v(1cm)
  #place(bottom, float:true)[#text(size: 12pt)[Baptiste Pires - 03/12/2025]]
]

#pagebreak()



= Exercice 1
== Question 1

Partons du principe qu'elles ne le soient pas. On souhaite parcourir la table des pages, on commence à  #ccode("pml4[0] = vaddr") où #cp("vaddr") contient l'*adresse virtuelle* de la table #ccode("pml3"). On va donc devoir traduire #cp("vaddr") en *adresse physique* #cp("paddr") pour pouvoir avoir l'adresse de #ccode("pml3"). On va donc devoir parcourir la table des pages (PML4 -> PML3 -> PML2 -> PML1) pour pouvoir faire la correspondance #cp("vaddr") -> #cp("paddr"). C'est récursif, on ne va jamais pouvoir quitter PML4.

Il faut donc que les adresses dans la table des pages soient des *addresses physiques*.

== Question 2 
#image("assets/image.png")

On a :
- 12 bits pour les flags
- 9 bits par niveaux -> 9 x 4 = 36 bits
- 12 bits pour l'extension de signe

Pour chaque niveau, nous avons donc $2^9 = 512$ entrées.

Pour la validité, il y a deux possibilités :
+ On est dans PML2 et le flag #ccode("0x80") est set (#ccode("HUGE_PAGE")")
+ On est dans PML1 (niveau 0)

Dans les deux cas il faut que le flags #ccode("0x1") soit set (valid bit).

== Question 3
Pour parcourir la table des pages, on commence par récupérer #ccode("cr3") qui contient l'adresse physique de table des pages du _processus courrant_. Il faut fait, on peut juste parcourir toutes les entrées de #cp("pml4"), si le bit #cp("valid") est set, on passe dans #cp("pml3") en appelant récusrivement la fonction #ccode("print_pgt(pml3_addr, level - 1)"). On fait de même pour les suivantes. 

Le code :
#ccodeml("
void print_pgt(paddr_t pml, uint8_t level)
{
	paddr_t *current_pml = (paddr_t *)pml;
	paddr_t pte_entry, next_pml_addr;

	if (level == 4) {
		printk(\"Initial pgt addr=%p, first_entry=%p\n\", current_pml, current_pml[0]);
	}

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
			printk(\"\\t\");

		printk(\"level=%u, index=%u, flags=%3lx, next_addr=%p, hugepage=%u\n\", level, i, pte_entry & 0xfff, next_pml_addr, !!PTE_IS_HUGE(pte_entry));
		
		if (!PTE_IS_HUGE(pte_entry))
			print_pgt(next_pml_addr, level - 1);
	}
}
")

= Exercice 2
== Question 1
On peut le faire de deux manières différentes.
+ La première est de définir des masques pour chaque niveau de la table des pages et de shifter l'adresse ensuite. Si je veux le premier niveau PML4, pour une adresse virtuelle #cp("vaddr"), je fais : 
  #ccode("pml4_index = (vaddr & 0xff8000000000) >> (12 + 9 x 3))"). Le masque #ccode("0xff8000000000") permet de ne garder que les bits correspondant à PML4, on shift de 12 bits pour l'offset et de 9 x 3 bits pour les niveaux PML3, PML2 et PML1, ce qui nous laisse avec l'index de PML4. Il suffit de faire ça pour toutes les tables avec le masque correspondant :
  #ccodeml("#define _PTE_MASK_PML1 0x1ff000
#define _PTE_MASK_PML2 0x3fe00000
#define _PTE_MASK_PML3 0x7fc0000000
#define _PTE_MASK_PML4 0xff8000000000")
+ Il est sinon possible de définir une macro (ou fonction) qui fait le travail de masquage et de shift en fonction du niveau demandé. Par exemple :
  #ccodeml("#define PTE_GET_INDEX_FOR_LVL(v, lvl) ((v) >> (12 + (9 * ((lvl) - 1))) & 0x1ff)") 
  Cette macro prend deux arguments : l'adresse virtuelle #cp("v") et le niveau #cp("lvl") (1 pour PML1, 2 pour PML2, etc). Elle shift l'adresse virtuelle de $12 + (9 * ("lvl" - 1))$ bits à droite pour enlever l'offset et les niveaux inférieurs (Si on veut le niveau 4, ça shiftera de $12 plus 9 times 3 = 39$, il reste bien les 9 derniers bits de l'index de PML4), puis applique un masque #cp("0x1ff") pour ne garder que les 9 bits correspondant à l'index du niveau demandé.

Je n'ai pas fait de mesure et j'ai pas regardé dans le noyau Linux ce qui était utilisé encore mais il y a sûrement une des deux approches qui est plus performante que l'autre. Le fait de ne pas faire de multiplications et de shifter est peut être plus efficace, à voir.

== Question 2
Il n'y a pas trop de piège ici, il faut seulement faire attention à positionner les bons flags : valid, user et rw. Le code :
#ccodeml("
void map_page(struct task *ctx, vaddr_t vaddr, paddr_t paddr)
{
	paddr_t *pgt_addr = (paddr_t *)ctx->pgt;
	paddr_t current_index;

	/* Pour tous les lvl intermediaires 4,3,2, on garde */
	for(uint8_t level = 4; level > 1; level--) {
	
		/* Calcul de l'index pour la pml courrante */
		current_index = PTE_GET_INDEX_FOR_LVL(vaddr, level);

		/* Si l'entree n'est pas valide on alloue une nouvelle page */
		if (!PTE_IS_VALID(pgt_addr[current_index])) {
			paddr_t new_page = alloc_page();
			memset((void *)new_page, 0, PAGE_SIZE);
			pgt_addr[current_index] = new_page | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;
		}

		/* On descend d'un niveau : risque de segfault ou pas ?*/
		pgt_addr = (paddr_t *)PTE_NEXT_ADDR(pgt_addr[current_index]);	
	}
	// Arrive ici, pgt_addr pointe sur la PML1

	current_index = PTE_GET_INDEX_PML1(vaddr);

	if (!PTE_IS_VALID(pgt_addr[current_index])) {
		pgt_addr[current_index] = paddr | PTE_FLAG_VALID | PTE_FLAG_USER | PTE_FLAG_RW;
	} else {
		printk(\"[warning] map_page: vaddr %p is already mapped\n\", vaddr);
		asm volatile (\"hlt\");
	}
}
")

= Exercice 3