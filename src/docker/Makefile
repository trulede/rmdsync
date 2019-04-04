
REPO = rmdsync
BUILD_DIR = build

.PHONY: default help docker clean cleanall start stop

default: docker

help:
	@echo "Please use \`make [<target>]' where <target> is one of:"
	@echo "  docker    Make the Docker Image (default)"
	@echo "  clean     Stop and delete the Docker Image"
	@echo "  cleanall  Remove all dangling Docker Images and Volumes"

cleanall: clean
	docker ps --filter status=dead --filter status=exited -aq | xargs -r docker rm -v
	docker images -qf dangling=true | xargs -r docker rmi
	docker images -q | xargs -r docker rmi
	docker volume ls -qf dangling=true | xargs -r docker volume rm

clean:
	-rm -rf $(BUILD_DIR)
	-if [ -n "$$(docker images -q $(REPO):latest)" ]; then docker rmi -f $(REPO):latest; fi

docker:
	mkdir -p $(BUILD_DIR)
	rsync -r ../rmdsync $(BUILD_DIR)
	docker build -t $(REPO):latest .

start:
	docker run --name $(REPO) -d --rm=true $(REPO):latest
	@echo "Wait 60 seconds for container to start ..."
	@for i in `seq 60` ; do \
		docker ps -f name=$(REPO) --format "{{.ID}} status={{.Status}}" | grep -q 'status=Up' ; \
		[ $$? = 0 ] && break ; \
		sleep 1 ; \
		echo "$$i ..." ; \
	done

stop:
	docker rm -f $$(docker ps -q -f name=$(REPO))
