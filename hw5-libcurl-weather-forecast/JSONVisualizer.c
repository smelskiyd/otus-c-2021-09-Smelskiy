//
// Created by Danya Smelskiy on 30.12.2021.
//

#include "JSONVisualizer.h"

void PrintIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
}

void PrintJsonArray(json_t* root, int indent) {
    printf("[\n");
    for (size_t i = 0; i < json_array_size(root); ++i) {
        if (i) {
            printf(",\n");
        }
        PrintIndent(indent + 1);
        RecursivelyPrintJson(json_array_get(root, i), indent + 1);
    }
    printf("\n");
    PrintIndent(indent);
    printf("]");
}

void PrintJsonString(json_t* root) {
    printf("\"%s\"", json_string_value(root));
}

void PrintJsonObject(json_t* root, int indent) {
    const char* key = NULL;
    json_t* value = NULL;
    int i = 0;

    printf("{\n");
    json_object_foreach(root, key, value) {
        if (i) {
            printf(",\n");
        }
        PrintIndent(indent + 1);
        printf("%s: ", key);
        RecursivelyPrintJson(value, indent + 1);
        ++i;
    }
    printf("\n");
    PrintIndent(indent);
    printf("}");
}

void PrintJsonTrue() {
    printf("TRUE");
}

void PrintJsonFalse() {
    printf("FALSE");
}

void PrintJsonInteger(json_t* root) {
    printf("%" JSON_INTEGER_FORMAT, json_integer_value(root));
}

void PrintJsonReal(json_t* root) {
    printf("%f", json_real_value(root));
}

void PrintJsonNull() {
    printf("NULL");
}

void RecursivelyPrintJson(json_t* root, int indent) {
    if (json_is_array(root)) {
        PrintJsonArray(root, indent);
    } else if (json_is_string(root)) {
        PrintJsonString(root);
    } else if (json_is_object(root)) {
        PrintJsonObject(root, indent);
    } else if (json_is_true(root)) {
        PrintJsonTrue();
    } else if (json_is_false(root)) {
        PrintJsonFalse();
    } else if (json_is_integer(root)) {
        PrintJsonInteger(root);
    } else if (json_is_real(root)) {
        PrintJsonReal(root);
    } else if (json_is_null(root)) {
        PrintJsonNull();
    } else {
        fprintf(stderr, "Wrong JSON format");
        exit(1);
    }
}
