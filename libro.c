#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "libro.h"

void searchBook(int socket) {
    char titolo[100];
    FILE *file = fopen("libreria.json", "r");
    struct json_object *parsed_json;
    struct json_object *libri;
    struct json_object *libro;
    size_t n_libri;

    recv(socket, titolo, 100, 0);

    char buffer[1024];
    fread(buffer, 1024, 1, file);
    fclose(file);

    parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, "libri", &libri);
    n_libri = json_object_array_length(libri);

    for (size_t i = 0; i < n_libri; i++) {
        libro = json_object_array_get_idx(libri, i);
        const char *titolo_libro = json_object_get_string(json_object_object_get(libro, "titolo"));

        if (strcmp(titolo, titolo_libro) == 0) {
            send(socket, "Book found", strlen("Book found"), 0);
            return;
        }
    }

    send(socket, "Book not found", strlen("Book not found"), 0);
}
