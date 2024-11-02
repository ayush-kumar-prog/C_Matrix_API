#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>

/* Allocate memory for the matrix content */
int matrix_allocate(matrix_t *m, int rows, int columns) {
    m->rows = rows;
    m->columns = columns;
    m->content = (int **)malloc(rows * sizeof(int *));
    if (m->content == NULL) {
        return -1; // Memory allocation failed
    }
    for (int i = 0; i < rows; i++) {
        m->content[i] = (int *)malloc(columns * sizeof(int));
        if (m->content[i] == NULL) {
            // Free previously allocated memory on failure
            for (int j = 0; j < i; j++) {
                free(m->content[j]);
            }
            free(m->content);
            return -1; // Memory allocation failed
        }
    }
    return 0; // Success
}

/* Free the allocated memory for the matrix content */
void matrix_free(matrix_t *m) {
    if (m->content != NULL) {
        for (int i = 0; i < m->rows; i++) {
            free(m->content[i]);
        }
        free(m->content);
        m->content = NULL;
    }
    m->rows = 0;
    m->columns = 0;
}

/* Initialize the matrix with a specific value */
void matrix_init_n(matrix_t *m, int n) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            m->content[i][j] = n;
        }
    }
}

/* Initialize the matrix with zeros */
void matrix_init_zeros(matrix_t *m) {
    matrix_init_n(m, 0);
}

/* Initialize the matrix as an identity matrix */
int matrix_init_identity(matrix_t *m) {
    if (m->rows != m->columns) {
        return -1; // Identity matrix must be square
    }
    matrix_init_zeros(m);
    for (int i = 0; i < m->rows; i++) {
        m->content[i][i] = 1;
    }
    return 0; // Success
}

/* Initialize the matrix with random values within a specified range */
int matrix_init_rand(matrix_t *m, int val_min, int val_max) {
    if (val_min > val_max) {
        return -1; // Invalid range
    }
    srand((unsigned int)time(NULL));
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            m->content[i][j] = val_min + rand() % (val_max - val_min + 1);
        }
    }
    return 0; // Success
}

/* Check if two matrices are equal */
int matrix_equal(matrix_t *m1, matrix_t *m2) {
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return 0; // Matrices have different dimensions
    }
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m1->columns; j++) {
            if (m1->content[i][j] != m2->content[i][j]) {
                return 0; // Matrices have different elements
            }
        }
    }
    return 1; // Matrices are equal
}

/* Sum two matrices */
int matrix_sum(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return -1; // Matrices have different dimensions
    }
    if (matrix_allocate(result, m1->rows, m1->columns) != 0) {
        return -1; // Memory allocation failed
    }
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m1->columns; j++) {
            result->content[i][j] = m1->content[i][j] + m2->content[i][j];
        }
    }
    return 0; // Success
}

/* Multiply a matrix by a scalar */
int matrix_scalar_product(matrix_t *m, int scalar, matrix_t *result) {
    if (matrix_allocate(result, m->rows, m->columns) != 0) {
        return -1; // Memory allocation failed
    }
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            result->content[i][j] = m->content[i][j] * scalar;
        }
    }
    return 0; // Success
}

/* Transpose a matrix */
int matrix_transposition(matrix_t *m, matrix_t *result) {
    if (matrix_allocate(result, m->columns, m->rows) != 0) {
        return -1; // Memory allocation failed
    }
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            result->content[j][i] = m->content[i][j];
        }
    }
    return 0; // Success
}

/* Multiply two matrices */
int matrix_product(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    if (m1->columns != m2->rows) {
        return -1; // Incompatible dimensions for matrix multiplication
    }
    if (matrix_allocate(result, m1->rows, m2->columns) != 0) {
        return -1; // Memory allocation failed
    }
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m2->columns; j++) {
            result->content[i][j] = 0;
            for (int k = 0; k < m1->columns; k++) {
                result->content[i][j] += m1->content[i][k] * m2->content[k][j];
            }
        }
    }
    return 0; // Success
}

/* Save a matrix to a file */
int matrix_dump_file(matrix_t *m, const char *output_file) {
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        return -1; // File opening failed
    }
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            fprintf(file, "%d ", m->content[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    return 0; // Success
}

int matrix_allocate_and_init_file(matrix_t *m, const char *input_file) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // File opening failed
    }
    int rows = 0, columns = 0;
    char line[1024];

    // First pass to determine the number of rows and columns
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) == 0 || line[0] == '\n') {
            continue; // Skip empty lines
        }
        if (rows == 0) {
            char *token;
            char *saveptr;
            token = strtok_r(line, " ", &saveptr);
            while (token) {
                columns++;
                token = strtok_r(NULL, " ", &saveptr);
            }
        }
        rows++;
    }

    fseek(file, 0, SEEK_SET);

    if (matrix_allocate(m, rows, columns) != 0) {
        fclose(file);
        perror("Error allocating matrix");
        return -1; // Memory allocation failed
    }

    // Second pass to read the matrix data
    int row = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) == 0 || line[0] == '\n') {
            continue; // Skip empty lines
        }
        int col = 0;
        char *token;
        char *saveptr;
        token = strtok_r(line, " ", &saveptr);
        while (token) {
            if (col < columns) {
                m->content[row][col] = atoi(token);
                col++;
            }
            token = strtok_r(NULL, " ", &saveptr);
        }
        row++;
    }

    fclose(file);
    return 0; // Success
}