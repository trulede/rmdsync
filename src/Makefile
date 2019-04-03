
DOCKER_DIR = docker


.PHONY: default help clean cleanall build test deploy start stop pytest

default: build

help:
	@echo "Please use \`make [<target>]' where <target> is one of:"
	@echo "  build     Build the Docker Image (default)"
	@echo "  test      Run tests against the Docker Image"
	@echo "  clean     Stop and delete the Docker Image"
	@echo "  cleanall  Remove all dangling Docker Images and Volumes"

clean:
	cd $(DOCKER_DIR) && $(MAKE) $@

cleanall: clean
	cd $(DOCKER_DIR) && $(MAKE) $@

build:
	cd $(DOCKER_DIR) && $(MAKE) docker

test:
	$(MAKE) start &&													\
	pytest -rx -x --tb=short ./test || ret=$$? &&						\
	$(MAKE) stop && exit $$ret

start:
	cd $(DOCKER_DIR) && $(MAKE) start

pytest:
	pytest -rx -x --tb=short ./test

stop:
	cd $(DOCKER_DIR) && $(MAKE) stop

deploy:
	@echo "Not Implemented!"
