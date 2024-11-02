#ifndef MATRIX_H
#define MATRIX_H

/* The data structure representing a matrix */
typedef struct {
    int rows;       /* number of rows */
    int columns;    /* number of columns */
    int **content;  /* matrix elements in a two dimensional array */
} matrix_t;

/* Set the rows and columns fields of a matrix object, and allocate memory for
 * its content. Return 0 on success, something else on failure */
int matrix_allocate(matrix_t *m, int rows, int columns);


/* Free a matrix object */
void matrix_free(matrix_t *m);


/* Initialise a previously allocated matrix with random integer numbers, taking
 * values between min and max. Return 0 on success, something else on failure */
int matrix_init_rand(matrix_t *m, int val_min, int val_max);


/* Initialise a previously allocated matrix by setting each element to n. */
void matrix_init_n(matrix_t *m, int n);


/* Initialise a previously allocated matrix by setting each element to 0. */
void matrix_init_zeros(matrix_t *m);


/* Initialise the matrix' elements to create an identity matrix. Return 0 on
 * success, something else on failure. */
int matrix_init_identity(matrix_t *m);


/* Return 1 if the two matrices are equal, i.e. they have the same number of
 * rows and columns, and same elements. Return 0 if the matrices are
 * different. */
int matrix_equal(matrix_t *m1, matrix_t *m2);


/* Sum 'm1' and 'm2' into result. matrix_sum should take care of allocating
 * result. Return 0 on success, something else on failure. */
int matrix_sum(matrix_t *m1, matrix_t *m2, matrix_t *result);


/* Multiply the matrix 'm' by the scalar 'scalar' and places the result into
 * 'result'. Should take care of the allocation of 'result'. Return 0 on
 * success, something else on failure. */
int matrix_scalar_product(matrix_t *m, int scalar, matrix_t *result);


/* Transpose 'm' into 'result'. Should take care of the allocation of 'result'.
 * Return 0 on success, something else on failure. */
int matrix_transposition(matrix_t *m, matrix_t *result);


/* Multiply 'm1' by 'm2' and place the result in 'result'. Should take care of
 * allocating 'result'. Return 0 on success and something else on failure. */
int matrix_product(matrix_t *m1, matrix_t *m2, matrix_t *result);

/* Save 'm' as a file, using the format described in the subject. Return 0
 * on success, something else on failure. If 'output_file' already exists,
 * it should be overwritten. */
int matrix_dump_file(matrix_t *m, const char *output_file);

/* Load a matrix from a file using the textual format described in the subject.
 * Should take care of allocating 'm'. Return 0 on success, something else on
 * failure. */
int matrix_allocate_and_init_file(matrix_t *m, const char *input_file);

#endif /* MATRIX_H */
