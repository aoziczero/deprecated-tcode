SUB_DIRS=./common\
	./buffer\
	./diagnostics\
	./io\
	./threading\
	./transport\

all: subdirs

subdirs:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir all ${ARGS}; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

clean:
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
		if [ $$? != 0 ]; then exit 1; fi; \
	donels
