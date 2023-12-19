#define BOOST_TEST_MODULE Tester
#include <matrix.hpp>
#include <boost/test/included/unit_test.hpp>

int add(int a, int b)
{
    return a + b;
}

BOOST_AUTO_TEST_CASE(empty_matrix_read)
{
    SparseMatrix matrix;
    BOOST_CHECK(matrix.readFromGrid(0, 0, "+X") == "");
    BOOST_CHECK(matrix.readFromGrid(0, 0, "-X") == "");
    BOOST_CHECK(matrix.readFromGrid(0, 0, "+Y") == "");
    BOOST_CHECK(matrix.readFromGrid(0, 0, "-Y") == "");
}

BOOST_AUTO_TEST_CASE(non_empty_matrix_read)
{
    SparseMatrix matrix;
    matrix.insertString(0, 0, "+X", "shreyashonnalli");
    BOOST_CHECK(matrix.readFromGrid(0, 0, "+X") == "shreyashonnalli");
    BOOST_CHECK(matrix.readFromGrid(0, 5, "+X") == "ashonnalli");
    BOOST_CHECK(matrix.readFromGrid(0, 10, "+X") == "nalli");

    BOOST_CHECK(matrix.readFromGrid(0, 0, "-X") == "s");
    BOOST_CHECK(matrix.readFromGrid(0, 5, "-X") == "ayerhs");
    BOOST_CHECK(matrix.readFromGrid(0, 10, "-X") == "nnohsayerhs");

    BOOST_CHECK(matrix.readFromGrid(0, 0, "-Y") == "s");
    BOOST_CHECK(matrix.readFromGrid(0, 0, "+Y") == "s");
    BOOST_CHECK(matrix.readFromGrid(0, 1, "-Y") == "h");
    BOOST_CHECK(matrix.readFromGrid(0, 1, "+Y") == "h");
}

BOOST_AUTO_TEST_CASE(empty_matrix)
{
    SparseMatrix matrix;
    matrix.insertString(0, 0, "+X", "shreyashonnalli");
    matrix.emptySparseMatrix();
    BOOST_CHECK(matrix.getSparseMatrix().size() == 0);
}

BOOST_AUTO_TEST_CASE(insert_to_matrix)
{
    SparseMatrix matrix;

    // testing corners valid
    BOOST_CHECK(matrix.insertString(0, 0, "+X", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(0, 0, "+Y", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "+X", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "-Y", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "-X", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "+Y", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "-X", "shreyashonnalli") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "-Y", "shreyashonnalli") == 1);

    // testing corners valid but bordering
    BOOST_CHECK(matrix.insertString(0, 0, "-X", "s") == 1);
    BOOST_CHECK(matrix.insertString(0, 0, "-Y", "s") == 1);
    BOOST_CHECK(matrix.insertString(0, 0, "-X", "sh") == -1);
    BOOST_CHECK(matrix.insertString(0, 0, "-Y", "sh") == -1);

    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "-X", "s") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "+Y", "s") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "-X", "sh") == -1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, 0, "+Y", "sh") == -1);

    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "+X", "s") == 1);
    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "-Y", "s") == 1);
    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "+X", "sh") == -1);
    BOOST_CHECK(matrix.insertString(0, UINT32_MAX - 1, "-Y", "sh") == -1);

    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "+X", "s") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "+Y", "s") == 1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "+X", "sh") == -1);
    BOOST_CHECK(matrix.insertString(UINT32_MAX - 1, UINT32_MAX - 1, "+Y", "sh") == -1);

    // testing invalid input
    BOOST_CHECK(matrix.insertString(UINT32_MAX, UINT32_MAX, "-X", "s") == -1);

    // random in the middle
    BOOST_CHECK(matrix.insertString(9746754, 16532, "-X", "shreyashonnalli") == 1);
}
