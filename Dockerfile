FROM ubuntu:latest
RUN apt-get update && apt-get install -y gcc make build-essential libjson-c-dev
WORKDIR /app
COPY . .
RUN gcc -o server server.c utente.c libro.c carrello.c -ljson-c
RUN gcc -o client client.c
EXPOSE 8080
CMD ["./server"]