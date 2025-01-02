# Usa l'immagine di base Ubuntu
FROM ubuntu:latest

# Abilita il repository universe e aggiorna i pacchetti
RUN apt-get update && apt-get install -y software-properties-common \
    && add-apt-repository universe \
    && apt-get update

# Installzioni varie necessarie
RUN apt-get install -y \
    gcc \
    build-essential \
    postgresql \
    postgresql-contrib \
    libpq-dev

# Imposta la directory di lavoro all'interno del container
WORKDIR /app

# Copia il contenuto del progetto dentro il container
COPY . .

# Compiliamo prima il server e poi il client
RUN gcc -o server server.c utente.c libro.c carrello.c -I/usr/include/postgresql -lpq

RUN gcc -o client client.c

EXPOSE 8080

CMD ["./server"]

