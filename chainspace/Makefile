all: start-nodes start-client-api

ps:
	ps aux | grep -v grep | grep chainspace | awk '{print $$2 " " $$11 " " $$12 " " $$13}'

check-port:
	lsof -i :$(port)

list-nodes:
	screen -list

build-jar:
	cd chainspacecore && mvn -Dversion=1.0-SNAPSHOT package assembly:single

dist:
	bash ./contrib/core-tools/build-node-dist.sh

start-nodes:
	bash ./contrib/core-tools/easystart.linux.sh

start-nodes-debug:
	bash ./contrib/core-tools/easystart.mac.debug.sh

tail-node:
	tail -f chainspacecore-0-0/screenlog.0

start-client-api:
	cd chainspacecore-0-0 && bash ./runclientservice.sh 0 0
	cd chainspacecore-1-0 && bash ./runclientservice.sh 1 0

path=/
curl-client-api:
	curl -v -H "Accept: application/json" -H "Content-Type: application/json" http://localhost:5000/api/1.0$(path) && echo "\n\n"

kill-all:
	ps aux | grep -v grep | grep chainspace | awk '{print $$2}' | xargs kill

docker:
	docker build --rm --tag chainspace:build1 -f build1.dockerfile .
	docker build --rm --tag chainspace:build2 -f build2.dockerfile .
	docker run -d -p 5000:5000 chainspace:build2
