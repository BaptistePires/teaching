DAT := dat/
BIN := bin/
PDF := pdf/

CC      := gcc
CCFLAGS := -Wall -Wextra -pedantic -O2
LDFLAGS := -lpthread

experiments := cacheline cachesize cacheway siblings cacheline-perf cachesize-perf
cacheline-param := 8 12 16 32 48 64 80 96 128
cachesize-param := 2048 4096 6144 8192 10240 12288 14336 16384 18432 20480 22528 24576 26624 28672 30720 32768 34816 36864 38912 40960 43008 45056 47104 49152 51200 53248 55296 57344 59392 61440 63488 65536 131072 196608 262144 327680 393216 458752 524288 589824 655360 720896 786432 851968 917504 983040 1048576 1114112 1179648 1245184 1310720 1376256 1441792 1507328 1572864 1638400 1703936 1769472 1835008 1900544 1966080 2031616 2097152 3597152 5097152 6597152 8097152 9597152 11097152 12582912 12597152 13000000 14500000 16000000 17500000 19000000
cacheway-param  := 1 2 4 8 12 14
siblings-param  := 1

# Extras
cacheline-perf-param := 8 16 32 64 128 256 512

all:

view:
	evince $^


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
	evince $$<

endef

$(foreach e, $(experiments), \
  $(eval $(call DATA_PATTERN, $(e))))



$(DAT) $(BIN) $(PDF):
	mkdir $@

clean:
	-rm -rf $(BIN) $(DAT) $(PDF)


.SECONDARY:
.NOTPARALLEL:
