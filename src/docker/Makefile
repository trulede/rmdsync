
REPO = rmdsync
BUILD_DIR = build

.PHONY: default help docker clean cleanall

default: docker

help:
	@echo "Please use \`make [<target>]' where <target> is one of:"
	@echo "  docker    Make the Docker Image (default)"
	@echo "  clean     Stop and delete the Docker Image"
	@echo "  cleanall  Remove all dangling Docker Images and Volumes"

cleanall: clean
	-docker rmi $$(docker images -q -f dangling=true)

clean:
	-rm -rf $(BUILD_DIR)
	-if [ -n "$$(docker images -q $(REPO):latest)" ]; then docker rmi -f $(REPO):latest; fi

docker:
	mkdir -p $(BUILD_DIR)/mods
	cp -r ../rmdsync/rmdsync.so $(BUILD_DIR)/mods/rmdsync.so
	-docker rmi -f $(REPO):latest
	docker build -t $(REPO):latest .
