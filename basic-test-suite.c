#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "matrix.h"
#include "unity.h"

/* By default this file will compile into a program executing the entire test
 * suite. You can also compile a program containing a single test by adding the
 * following to the compiler invocation commend:
 * -D__TEST_NAME__=<test name>
 * For example:
 * gcc -D__TEST_NAME__=load_from_file basic-test-suite.c ...
 */

void setUp() {}

void tearDown() {}

/*******************************************************************************
 * Allocation test
 ******************************************************************************/

void allocate() {
    matrix_t m;
    int rows = 10, cols = 5;

    m.content = NULL;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));

    TEST_ASSERT_TRUE(m.content);

    TEST_ASSERT_TRUE(m.rows = rows);
    TEST_ASSERT_TRUE(m.columns = cols);

    matrix_free(&m);
}

/*******************************************************************************
 * Initialization tests
 ******************************************************************************/

void init_rand() {
    matrix_t m;
    int rows = 10, cols = 5;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));

    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            m.content[i][j] = INT32_MAX;

    matrix_init_rand(&m, val_min, val_max);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++) {
            TEST_ASSERT_TRUE(m.content[i][j] != INT32_MAX);
            TEST_ASSERT_TRUE(m.content[i][j] >= val_min);
            TEST_ASSERT_TRUE(m.content[i][j] <= val_max);
        }

    matrix_free(&m);
}

void init_zeros() {
    matrix_t m;
    int rows = 10, cols = 5;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));

    matrix_init_zeros(&m);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            TEST_ASSERT_TRUE(m.content[i][j] == 0);

    matrix_free(&m);
}

void init_identity() {
    matrix_t m;
    int rows = 10, cols = 10;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));

    TEST_ASSERT_EQUAL_INT32(0, matrix_init_identity(&m));
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            if(i == j)
                TEST_ASSERT_TRUE(m.content[i][j] == 1);
            else
                TEST_ASSERT_TRUE(m.content[i][j] == 0);

    matrix_free(&m);
}

/*******************************************************************************
 * Matrix operations tests
 ******************************************************************************/

void equal() {
    matrix_t m1, m2;
    int rows = 10, cols=5;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m1, rows, cols));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m2, rows, cols));

    matrix_init_zeros(&m1);
    matrix_init_zeros(&m2);
    TEST_ASSERT_TRUE(matrix_equal(&m1, &m2));

    matrix_init_identity(&m1);
    matrix_init_identity(&m2);
    TEST_ASSERT_TRUE(matrix_equal(&m1, &m2));

    m1.content[0][0] = 100;
    TEST_ASSERT_FALSE(matrix_equal(&m1, &m2));

    matrix_free(&m2);
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m2, rows, cols-1));
    matrix_init_rand(&m2, val_min, val_max);
    TEST_ASSERT_EQUAL_INT32(0, matrix_equal(&m1, &m2));

    matrix_free(&m1);
    matrix_free(&m2);
}

void sum() {
    matrix_t m1, m2, result;
    int rows=10, cols=5;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m1, rows, cols));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m2, rows, cols));
    matrix_init_rand(&m1, val_min, val_max);
    matrix_init_zeros(&m2);

    TEST_ASSERT_EQUAL_INT32(0, matrix_sum(&m1, &m2, &result));

    TEST_ASSERT_TRUE(matrix_equal(&m1, &result));

    matrix_free(&m1);
    matrix_free(&m2);
    matrix_free(&result);
}

void scalar_product() {
    matrix_t m, expected, result;
    int rows = 5, cols = 10;
    int scalar = 0;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&expected, rows, cols));
    matrix_init_rand(&m, val_min, val_max);
    matrix_init_zeros(&expected);

    TEST_ASSERT_EQUAL_INT32(0, matrix_scalar_product(&m, scalar, &result));

    TEST_ASSERT_TRUE(matrix_equal(&result, &expected));

    matrix_free(&m);
    matrix_free(&result);
    matrix_free(&expected);
}

void transposition() {
    matrix_t m, temp, result;
    int rows=10, cols=5;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));
    matrix_init_rand(&m, val_min, val_max);

    TEST_ASSERT_EQUAL_INT32(0, matrix_transposition(&m, &temp));
    TEST_ASSERT_EQUAL_INT32(0, matrix_transposition(&temp, &result));

    TEST_ASSERT_TRUE(matrix_equal(&result, &m));

    matrix_free(&m);
    matrix_free(&result);
    matrix_free(&temp);
}

void product() {
    matrix_t m1, m2, result;
    int rows=5, cols=10;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m1, rows, cols));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m2, cols, cols));
    matrix_init_rand(&m1, val_min, val_max);
    matrix_init_identity(&m2);

    TEST_ASSERT_EQUAL_INT32(0, matrix_product(&m1, &m2, &result));

    TEST_ASSERT_TRUE(matrix_equal(&result, &m1));

    matrix_free(&m1);
    matrix_free(&m2);
    matrix_free(&result);
}

/*******************************************************************************
 * File I/O tests
 ******************************************************************************/

void dump_file() {
    matrix_t m;
    char *file = "matrix.txt";
    int rows = 10, cols = 5;
    int val_min = 0, val_max = 100;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m, rows, cols));
    matrix_init_rand(&m, val_min, val_max);

    remove(file);
    TEST_ASSERT_EQUAL_INT32(0, matrix_dump_file(&m, file));
    TEST_ASSERT_EQUAL_INT32(0, access(file, F_OK)); /* check if file was created */

    remove(file);
    matrix_free(&m);
}

void load_from_file() {
    matrix_t m;
    char *file = "matrix-samples/valid.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m, file));

    for(int i=0; i<m.rows; i++)
        for(int j=0; j<m.columns; j++)
            TEST_ASSERT_EQUAL_INT32(i*m.columns+j, m.content[i][j]);

    matrix_free(&m);
}

void dump_and_load_from_file() {
    matrix_t m1, m2;
    int rows = 10, cols = 5;
    int val_min = 0, val_max = 100;
    char *file = "matrix.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate(&m1, rows, cols));
    matrix_init_rand(&m1, val_min, val_max);
    TEST_ASSERT_EQUAL_INT32(0, matrix_dump_file(&m1, file));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m2, file));

    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            TEST_ASSERT_TRUE(m1.content[i][j] == m2.content[i][j]);

    remove(file);
    matrix_free(&m1);
    matrix_free(&m2);
}

/*******************************************************************************
 * Operations + File I/O tests
 ******************************************************************************/

void equal_file() {
    matrix_t m1, m2;
    char *f1 = "matrix-samples/m-5-10-a.txt";
    char *f2 = "matrix-samples/m-5-10-a-copy.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m1, f1));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m2, f2));

    TEST_ASSERT_TRUE(matrix_equal(&m1, &m2));

    matrix_free(&m1);
    matrix_free(&m2);
}

void sum_file() {
    matrix_t m1, m2, result, expected;
    char *f1 = "matrix-samples/m-5-10-a.txt";
    char *f2 = "matrix-samples/m-5-10-b.txt";
    char *f3 = "matrix-samples/sum.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m1, f1));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m2, f2));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&expected, f3));

    TEST_ASSERT_EQUAL_INT32(0, matrix_sum(&m1, &m2, &result));

    TEST_ASSERT_TRUE(matrix_equal(&expected, &result));

    matrix_free(&m1);
    matrix_free(&m2);
    matrix_free(&expected);
    matrix_free(&result);
}

void scalar_product_file() {
    matrix_t m, result, expected;
    char *f1 = "matrix-samples/m-5-10-a.txt";
    char *f2 = "matrix-samples/scalar-product.txt";
    int scalar = 42;

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m, f1));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&expected, f2));

    TEST_ASSERT_EQUAL_INT32(0, matrix_scalar_product(&m, scalar, &result));

    TEST_ASSERT_TRUE(matrix_equal(&expected, &result));

    matrix_free(&m);
    matrix_free(&expected);
    matrix_free(&result);
}

void transposition_file() {
    matrix_t m, result, expected;
    char *f1 = "matrix-samples/m-5-10-a.txt";
    char *f2 = "matrix-samples/transposition.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m, f1));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&expected, f2));

    TEST_ASSERT_EQUAL_INT32(0, matrix_transposition(&m, &result));

    TEST_ASSERT_TRUE(matrix_equal(&expected, &result));

    matrix_free(&m);
    matrix_free(&expected);
    matrix_free(&result);
}

void product_file() {
    matrix_t m1, m2, result, expected;
    char *f1 = "matrix-samples/m-5-10-a.txt";
    char *f2 = "matrix-samples/m-10-5-a.txt";
    char *f3 = "matrix-samples/matrix-product.txt";

    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m1, f1));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&m2, f2));
    TEST_ASSERT_EQUAL_INT32(0, matrix_allocate_and_init_file(&expected, f3));

    TEST_ASSERT_EQUAL_INT32(0, matrix_product(&m1, &m2, &result));

    TEST_ASSERT_TRUE(matrix_equal(&expected, &result));

    matrix_free(&m1);
    matrix_free(&m2);
    matrix_free(&expected);
    matrix_free(&result);
}

int main() {
    UNITY_BEGIN();

#ifdef __TEST_NAME__

    RUN_TEST(__TEST_NAME__);

#else

    /* Matrix allocation test */
    RUN_TEST(allocate);

    /* Standard initializations of various sizes */
    RUN_TEST(init_rand);
    RUN_TEST(init_zeros);
    RUN_TEST(init_identity);

    /* Equality test */
    RUN_TEST(equal);

    /* Basic operations tests */
    RUN_TEST(sum);
    RUN_TEST(scalar_product);
    RUN_TEST(transposition);
    RUN_TEST(product);

    /* Basic dump test, just verifies the operation completes successfully
     * and that the file is there but does not check the validity of the
     * file's content */
    RUN_TEST(dump_file);

    /* Basic load from file tests with a strict format */
    RUN_TEST(load_from_file);

    /* Dump then reload test */
    RUN_TEST(dump_and_load_from_file);

    /* Equality tests on matrices loaded from files */
    RUN_TEST(equal_file);

    /* Operations test on matrices loaded from files */
    RUN_TEST(sum_file);
    RUN_TEST(scalar_product_file);
    RUN_TEST(transposition_file);
    RUN_TEST(product_file);

#endif /* __TEST_NAME__ */

    return UNITY_END();
}
