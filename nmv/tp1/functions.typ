#import "style.typ": *

#let __todo(body) = {
  text(weight: "bold", red, "TODO: " + body)
}

#let __note(body) = {
  text(blue, emph("TODO: " + body))
}

#let _note(title, body) = {
  showybox(
  title: title,
	title-style: (
		color: black,
		weigth: "extrabold",
	),
  frame: (
    border-color: purple,
    title-color: purple.lighten(35%),
    body-color: purple.lighten(95%),
    footer-color: purple.lighten(80%)
  ),
)[#body]
}