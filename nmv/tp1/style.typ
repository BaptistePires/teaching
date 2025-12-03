#import "imports.typ": *
#import "@preview/showybox:2.0.4": showybox
// #import "sys"


#let doc-header(
  title: none,
  authors: (),
  doc,
) = {
  set align(center)
  text(17pt, title)

  let count = authors.len()
  let ncols = calc.min(count, 3)
  grid(
    columns: (1fr,) * ncols,
    row-gutter: 24pt,
    ..authors.map(author => [
      #author.name \
      #author.affiliation \
      #link("mailto:" + author.email)
    ])
  )
  show "sched_ext": smallcaps
  doc
}
#let doc-body(
  doc,
) = {

	// Page setup like latex
	// set page(margin: 70pt, number-align: center, numbering: "- 1 -")
	set par(
		leading: 0.55em,
		spacing: 1.5em,
		first-line-indent: 1.8em,
		justify: true,
	)
	set text(font: "New Computer Modern")
	// show raw: set text(font: "New Computer Modern Mono", size: 11pt)
	
	set align(left)

	// Headings
	set heading(numbering: "1. ", hanging-indent: auto)	
	show heading: set block(above: 1.4em, below: 1em)
	show heading: it => {showybox(frame:(radius:10pt, thickness: (left:1pt, bottom: 1pt,), border-color:rgb("#8565c4")))[#it]}
	  

	// specifis formats
	// show regex("\bscx"): txt => [
	// 	#smallcaps(text(fill:rgb("#8565c4"), txt))
	// ]

	show regex("\blb\b"): smallcaps
	show regex("\bllc\b"): smallcaps
	show regex("\bsmt\b"): smallcaps
	
	
	show "dsq": smallcaps
	show "rbtree": smallcaps
	show "cpu": smallcaps
	// show "lb": smallcaps
	show "ebpf": emph("eBPF")
	show "cgroup": smallcaps
	show "cgroups": smallcaps
	show "numa": smallcaps
	show "bpf": smallcaps
	
	text(12pt, doc)
}


#let ccode(code) = {
	raw(code, theme:auto, lang: "c")
}

#let ccodeml(code) = {
	block(raw(code, lang: "c", ), fill: rgb(220, 220, 240), radius: 10pt, inset: 10pt, width: 100%, )
}

#let cp(to_color) = {
	text(rgb("#790cc2"), to_color)
}

