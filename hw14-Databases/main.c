//
// Created by Danya Smelskiy on 08.02.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include <libpq-fe.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

#define SQR(x) ((x) * (x))

const char* localhost_addr = "127.0.0.1";

static char buffer[MAX_COMMAND_LENGTH];

PGconn* ConnectToDatabase(const char* database_name) {
    char user_name[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    printf("Write username: ");
    scanf("%s", user_name);
    printf("Write password: ");
    scanf("%s", password);

    snprintf(buffer, MAX_COMMAND_LENGTH, "user=%s password=%s host=%s dbname=%s",
             user_name, password, localhost_addr, database_name);
    PGconn* conn = PQconnectdb(buffer);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Failed to connect to database '%s'.\n", database_name);
        if (conn != NULL) {
            PQfinish(conn);
        }
        exit(EXIT_FAILURE);
    }
    printf("Successfully connected to Postgres database '%s'\n", database_name);

    return conn;
}

void VerifyColumnType(PGconn* conn, const char* table_name, const char* column_name) {
    snprintf(buffer, MAX_COMMAND_LENGTH, "SELECT    data_type\n"
                                         "FROM      information_schema.columns\n"
                                         "WHERE     table_name = '%s'\n"
                                         "AND       column_name = '%s'\n"
                                         "ORDER BY  ordinal_position;",
                                         table_name, column_name);

    PGresult* result = PQexec(conn, buffer);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to execute command: '%s'.\n", buffer);
        if (result != NULL) {
            PQclear(result);
        }
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    int rows = PQntuples(result);
    if (rows == 0) {
        fprintf(stderr, "Column `%s` wasn't found in table `%s`.\n", column_name, table_name);
        PQclear(result);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    } else if (rows > 1) {
        fprintf(stderr, "There are more than 1 columns with name `%s` in table `%s`.\n", column_name, table_name);
        PQclear(result);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    assert(PQnfields(result) == 1);

    char* column_type = PQgetvalue(result, 0, 0);
    if (strcmp(column_type, "integer") != 0) {
        fprintf(stderr, "Input column must be of integer type, but it contains type '%s'!\n", column_type);
        PQclear(result);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    PQclear(result);
}

long* GetColumnValues(PGconn* conn, const char* table_name, const char* column_name, int* n) {
    snprintf(buffer, MAX_COMMAND_LENGTH, "SELECT %s FROM %s", column_name, table_name);
    PGresult* result = PQexec(conn, buffer);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to execute command: '%s'.\n", buffer);
        if (result != NULL) {
            PQclear(result);
        }
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    assert(PQnfields(result) == 1);

    *n = PQntuples(result);
    printf("Table has %d rows.\n", *n);

    long* arr = (long*)malloc((*n) * sizeof(long));
    for (int i = 0; i < *n; ++i) {
        char* current_value = PQgetvalue(result, i, 0);
        char* first_invalid = NULL;
        long value = strtol(current_value, &first_invalid, 10);

        if (current_value == NULL || current_value[0] == '\0' || *first_invalid != '\0') {
            fprintf(stderr, "Wrong format of column value.\n");
            PQclear(result);
            PQfinish(conn);
            exit(EXIT_FAILURE);
        }

        arr[i] = value;
    }

    PQclear(result);

    return arr;
}

void CalcStatistics(const long* arr, int n) {
    long min = LONG_MAX;
    long max = LONG_MIN;
    long sum = 0;

    for (int i = 0; i < n; ++i) {
        long cur = arr[i];
        if (cur < min) {
            min = cur;
        }
        if (cur > max) {
            max = cur;
        }
        sum += cur;
    }

    printf("Minimum value: %ld\n", min);
    printf("Maximum value: %ld\n", max);
    printf("Sum of all values: %ld\n", sum);

    long avg = sum / (long)n;
    printf("Average value: %ld\n", avg);

    long variance_sum = 0;
    for (int i = 0; i < n; ++i) {
        long cur = arr[i];
        variance_sum += SQR(cur - avg);
    }
    variance_sum /= (long)n;

    printf("Variance (S): %f\n", sqrt((double)variance_sum));
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly 3 input arguments in the following order:\n"
                        "1. Database name;\n"
                        "2. Table name;\n"
                        "3. Column name;\n");
        return 1;
    }
    const char* database_name = argv[1];
    const char* table_name = argv[2];
    const char* column_name = argv[3];
    printf("Database name: '%s'\n", database_name);
    printf("Table name: '%s'\n", table_name);
    printf("Column name: '%s'\n", column_name);

    PGconn* conn = ConnectToDatabase(database_name);

    VerifyColumnType(conn, table_name, column_name);

    int n;
    long* arr = GetColumnValues(conn, table_name, column_name, &n);

    CalcStatistics(arr, n);

    free(arr);
    PQfinish(conn);
    return 0;
}
