all: solo solo-dev

solo-dev: solo Dockerfile.solo-dev
	docker build -t solo-dev -f Dockerfile.solo-dev .

solo: Dockerfile.solo
	docker build -t solo -f Dockerfile.solo .

