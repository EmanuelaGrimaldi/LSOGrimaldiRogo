# Usa l'immagine di base di Ubuntu
FROM ubuntu:20.04

# Installare i pacchetti necessari
RUN apt-get update && apt-get install -y gcc

# Creare una directory per l'applicazione
WORKDIR /app

# Copiare i file sorgente nel container
COPY server.c .
COPY client.c .

# Compilare il codice C
RUN gcc -o server server.c
RUN gcc -o client client.c

# Esportare la porta che il server utilizzerà
EXPOSE 8080

# Il comando di default esegue il server
CMD ["./server"]