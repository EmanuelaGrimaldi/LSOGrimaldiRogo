CREATE TABLE utente(
    nome VARCHAR(50) NOT NULL,
    email VARCHAR(50) PRIMARY KEY,
    password VARCHAR(30) NOT NULL
);

CREATE TABLE libro(
    isbn INT PRIMARY KEY,
    titolo VARCHAR(50) NOT NULL,
    categoria VARCHAR(50),
    copieTotali INT NOT NULL,
    totCopiePrestate INT
);

CREATE TABLE prestito(
    isbnPrestito INT,
    emailPrestito VARCHAR(50),
    dataPrestito VARCHAR(10),
    dataRestituzione VARCHAR(10),

	 FOREIGN KEY(isbnPrestito) REFERENCES libro(isbn),
	 FOREIGN KEY(emailPrestito) REFERENCES utente(email)

);

CREATE TABLE carrello(
    isbnCarrello INT,
    emailCarrello VARCHAR(50),

	FOREIGN KEY(isbnCarrello) REFERENCES libro(isbn),
	FOREIGN KEY(emailCarrello) REFERENCES utente(email)
);

CREATE TABLE Kvalue(
    k INT 
);

INSERT INTO Kvalue (k)
VALUES(3);

INSERT INTO utente (nome, email, password)
VALUES ('Emy Bianchi', 'EmyMail@mail.com', 'psw00');

INSERT INTO utente (nome, email, password)
VALUES ('Francesco Bianchi', 'Framail@mail.com', 'psw00');

INSERT INTO utente (nome, email, password)
VALUES ('Mario Rossi', 'Mariomail@mail.com', 'psw00');

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 1111, 'libro di Storia', 'Narrativa', 7, 2);

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 2222, 'libro di Geografia', 'Scienze', 7, 1);

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 3333, 'libro di Arte', 'Narrativa', 5, 1);

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 4444, 'libro di Storia Romana', 'Fantascienza', 10, 2);

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 5555, 'libro di Fisica', 'Scienze', 7, 0);

INSERT INTO libro ( isbn, titolo, categoria, copieTotali, totCopiePrestate)
VALUES( 6666, 'libro di Matematica', 'Matematica', 10, 0);

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (1111, 'EmyMail@mail.com', '25/09', '25/12');

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (1111,'Framail@mail.com', '05/09', '05/12');

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (2222, 'EmyMail@mail.com', '25/09', '25/12');

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (3333, 'Mariomail@mail.com', '13/08', '13/11');

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (4444,'Framail@mail.com', '20/09', '20/12');

INSERT INTO prestito (isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
VALUES (4444,'Mariomail@mail.com', '20/09', '20/12');