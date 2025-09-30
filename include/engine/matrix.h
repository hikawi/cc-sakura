/**
 * \file engine/matrix.h
 *
 * Provides physics-related for calculations on matrices.
 */

#pragma once

#include "sdl/sdl_log.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace ccsakura
{

/**
 * A matrix consisting of m rows and n columns.
 */
template <size_t m, size_t n>
    requires(m > 0 && n > 0)
struct matrix
{
    double data[m][n] = {};

    /**
     * An all-zero matrix.
     */
    matrix()
    {
    }

    /**
     * Constructs a matrix from another matrix.
     *
     * \param other the other matrix to copy from
     */
    matrix(const matrix<m, n> &other)
    {
        std::copy_n(data, m * n, other.data);
    }

    /**
     * Copies by assignment from another matrix.
     *
     * \param other the other matrix to copy from
     * \return this matrix
     */
    matrix &operator=(const matrix<m, n> &other)
    {
        std::copy_n(data, m * n, other.data);
        return *this;
    }

    /**
     * Sets the value of an element in the matrix.
     *
     * \param row the row of the matrix (top is 0)
     * \param col the column of the matrix (left is 0)
     * \param val the value to set to
     */
    void set(const size_t row, const size_t col, const double val)
    {
        if (row >= m || col >= n)
        {
            sdl::log_critical("{},{} can not be greater or equal to {},{}", row, col, m, n);
            throw std::out_of_range("(x, y) can not be larger than matrix");
        }

        data[row][col] = val;
    }

    /**
     * Retrieves the value of a matrix's element.
     *
     * \param row the row of the matrix (top is 0)
     * \param col the column of the matrix (left is 0)
     * \return the value at that coordinate
     */
    double at(const size_t row, const size_t col) const
    {
        if (row >= m || col >= n)
        {
            sdl::log_critical("{},{} can not be greater or equal to {},{}", row, col, m, n);
            throw std::out_of_range("(x, y) can not be larger than matrix");
        }

        return data[row][col];
    }

    /**
     * Transposes this matrix in place.
     */
    void transpose() noexcept
    {
        *this = transposed();
    }

    /**
     * Returns a transposed matrix of this one.
     *
     * \return a transposed matrix
     */
    matrix<n, m> transposed() const noexcept
    {
        matrix<n, m> result;
        for (size_t y = 0; y < m; y++)
        {
            for (size_t x = 0; x < n; x++)
            {
                result.set(x, y, at(y, x));
            }
        }
        return result;
    }

    /**
     * Matrix multiplication.
     *
     * The other matrix to multiply with must have the number of rows match this matrix's number of columns.
     *
     * \param other the other matrix to multiply with
     * \return the resulting multiplication
     */
    template <size_t p> matrix<m, p> operator*(const matrix<n, p> &other) const noexcept
    {
        matrix<m, p> result;
        for (size_t row_idx = 0; row_idx < m; row_idx++)
        {
            for (size_t col_idx = 0; col_idx < p; col_idx++)
            {
                // We know the row and the col to multiply together have n elements.
                double sum = 0;
                for (size_t i = 0; i < n; i++)
                {
                    sum += at(row_idx, i) * other.at(i, col_idx);
                }
                result.set(row_idx, col_idx, sum);
            }
        }

        return result;
    }

    /**
     * Retrieves an identity matrix.
     *
     * This function is only defined for square matrices.
     *
     * \return an identity matrix
     */
    static matrix<m, n> identity() noexcept
        requires(m == n)
    {
        matrix<m, n> mat;
        for (size_t i = 0; i < m; i++)
        {
            mat.set(i, i, 1);
        }
        return mat;
    }
};

} // namespace ccsakura
