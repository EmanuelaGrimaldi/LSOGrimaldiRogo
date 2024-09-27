# Usa l'immagine di base Ubuntu
FROM ubuntu:latest

# Abilita il repository universe e aggiorna i pacchetti
RUN apt-get update && apt-get install -y software-properties-common \
    && add-apt-repository universe \
    && apt-get update

# Installa gcc, build-essential, PostgreSQL e la libreria di sviluppo libpq-dev
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

# Compila il server, specificando il percorso per le intestazioni libpq
RUN gcc -o server server.c utente.c libro.c carrello.c -I/usr/include/postgresql -lpq

# Compila il client
RUN gcc -o client client.c

# Espone la porta 8080 per consentire le connessioni al server
EXPOSE 8080

# Comando per avviare il server quando il container parte
CMD ["./server"]