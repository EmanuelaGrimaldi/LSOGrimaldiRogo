FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    postgresql postgresql-contrib libpq-dev

WORKDIR /app

COPY . .

RUN gcc -o server server.c utente.c libro.c carrello.c -lpq

RUN gcc -o client client.c

EXPOSE 8080

CMD ["./server"]