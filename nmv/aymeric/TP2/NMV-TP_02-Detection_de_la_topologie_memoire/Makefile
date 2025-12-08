DAT := dat/
BIN := bin/
PDF := pdf/

CC      := gcc
CCFLAGS := -Wall -Wextra -pedantic -O2
LDFLAGS := -lpthread -lm

experiments := cacheway
# cacheline cachesize cacheway siblings
# cacheline-param := 8 16 32 48 64 80 96 128 160 192 256 320 384
cacheline-param := 8 16 32 48 64 80 96
# cacheline-param := 8 12 16 32 48 64 80 96 128
# Parfait pour L1 : coupure entre 48Kio et 52Kio
# cachesize-param := 0x5000 0x6000 0x7000 0x8000 0x9000 0xA000 0xB000 0xC000 0xD000 0x10000 0x18000 0x20000
# L2 : devient constant à 2048 Kio, augmente entre 1024Kio et 2048Kio
cachesize-param := 0x60000 0x70000 0x80000 0x90000 0xA0000 0xE0000 0x100000 0x140000 0x180000 0x1A0000 0x200000 0x240000 0x280000
# Degré d'associativité de 12
cacheway-param  := 1 2 4 8 12 16 32
siblings-param  := 1

PDFVIEWER := mupdf

all:

view:
	$(PDFVIEWER) $^


define EXPERIMENT_PATTERN

  exe: $$(BIN)$(strip $(1))-$(strip $(2))

  $$(BIN)$(strip $(1))-$(strip $(2)): $(1).c $$(wildcard *.h) | $$(BIN)
	$$(CC) $$(CCFLAGS) -DPARAM=$(strip $(2)) $$< -o $$@ $$(LDFLAGS)

  $$(DAT)$(strip $(1)).dat: $$(BIN)$(strip $(1))-$(strip $(2)) | $$(DAT)

endef

$(foreach e, $(experiments), \
  $(foreach p, $($(e)-param), \
    $(eval $(call EXPERIMENT_PATTERN, $(e), $(p)))))


define DATA_PATTERN

  $$(DAT)$(strip $(1)).dat:
	for b in $$^ ; do echo "running $$$$b" >&2 ; ./$$$$b ; done > $$@

  $$(DAT)$(strip $(1)).sort.dat: $$(DAT)$(strip $(1)).dat | $$(DAT)
	sort -g $$< > $$@

  $$(PDF)$(strip $(1)).pdf: $(strip $(1)).sh $$(DAT)$(strip $(1)).sort.dat \
                          | $$(PDF)
	./$$^ $$@

  all: $$(PDF)$(strip $(1)).pdf

  view: $$(PDF)$(strip $(1)).pdf

  $(strip $(1)): $$(PDF)$(strip $(1)).pdf
	$(PDFVIEWER) $$<

endef

$(foreach e, $(experiments), \
  $(eval $(call DATA_PATTERN, $(e))))



$(DAT) $(BIN) $(PDF):
	mkdir $@

clean:
	-rm -rf $(BIN) $(DAT) $(PDF) GPATH GTAGS GRTAGS


.SECONDARY:
.NOTPARALLEL:
