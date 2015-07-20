#ifndef WORKERTHREAD_HPP
#define WORKERTHREAD_HPP

#include <thread>
#include <iostream>
#include <memory.h>

template <size_t dimension>
class WorkerThread
{
public:
    WorkerThread(unsigned from, unsigned to);
    unsigned numberOfSolutions() const;
    void solve ();
    static std::thread work (WorkerThread<dimension> *worker);

private:
    static constexpr unsigned s_square = dimension * dimension;
    static constexpr unsigned s_magicSum = (dimension * (s_square + 1)) >> 1;
    static constexpr unsigned s_limit = dimension - 1;
    static constexpr unsigned s_diag_limit = dimension - 2;

    unsigned m_numberOfSolutions;
    unsigned m_content[dimension][dimension];
    bool m_usedNumbers [s_square + 1];
    unsigned m_sourceRange;
    unsigned m_destRange;

    void backtrack (unsigned row, unsigned column, unsigned sum);
    void lastDiag ();
    void backtrackLastRow (unsigned i);
    void createMainDiag (unsigned i, unsigned sum);

    bool checkMainDiag () const;
    bool checkSecDiag ()  const;
    bool checkRow (const size_t &row) const;
    bool checkColumn (const size_t &column) const;
};

template <size_t dimension>
WorkerThread<dimension>::WorkerThread(unsigned from, unsigned to)
    : m_numberOfSolutions (0),
      m_sourceRange (from),
      m_destRange (to)
{
    memset(m_usedNumbers, false, (s_square) * sizeof (bool));
    for (unsigned i = 0; i < dimension; ++i)
    {
        memset(m_content[i], 0, dimension * sizeof (unsigned));
    }
}

template <size_t dimension>
unsigned WorkerThread<dimension>::numberOfSolutions() const
{
    return m_numberOfSolutions;
}

template <size_t dimension>
void WorkerThread<dimension>::backtrack(unsigned row, unsigned column, unsigned sum)
{
    if (row == s_limit && column == 0)
    {
        lastDiag();
        return;
    }

    if (row == column)
    {
        backtrack(row, column + 1, sum + m_content[row][column]);
        return;
    }

    unsigned s = 0;

    for (unsigned j = 0; j < s_square; ++j)
    {
        if (m_usedNumbers[j])
        {
            continue;
        }

        if ((s = sum + j + 1) > s_magicSum)
        {
            continue;
        }

        m_content[row][column] = j + 1;
        m_usedNumbers[j] = true;

        if (column == s_limit)
        {
            if (checkRow(row))
            {
                backtrack(row + 1, 0, 0);
            }
        }
        else
        {
            backtrack(row, column + 1, s);
        }

        m_usedNumbers[j] = false;
    }
}

template <size_t dimension>
void WorkerThread<dimension>::solve()
{
    for (unsigned root = m_sourceRange; root < m_destRange; ++root)
    {
        if (m_usedNumbers[root])
        {
            continue;
        }
        m_usedNumbers[root] = true;
        const unsigned root_val = root + 1;
        m_content[0][0] = root_val;
        createMainDiag(1, root_val);
        m_usedNumbers[root] = false;
    }
}

template <size_t dimension>
bool WorkerThread<dimension>::checkMainDiag() const
{
    unsigned sum = 0;
    for (unsigned i = 0 ; i < dimension; ++i)
    {
        sum += m_content[i][i];
    }
    return sum == s_magicSum;
}

template <size_t dimension>
bool WorkerThread<dimension>::checkSecDiag() const
{
    unsigned sum = 0;

    for (unsigned i = 0; i < dimension; ++i)
    {
        sum += m_content[s_limit - i][i];
    }

    return sum == s_magicSum;
}

template <size_t dimension>
bool WorkerThread<dimension>::checkRow(const size_t &row) const
{
    unsigned sum = 0;

    for (unsigned i = 0; i < dimension; ++i)
    {
        sum += m_content[row][i];
    }

    return sum == s_magicSum;
}

template <size_t dimension>
bool WorkerThread<dimension>::checkColumn(const size_t &column) const
{
    unsigned sum = 0;

    for (unsigned i = 0; i < dimension; ++i)
    {
        sum += m_content[i][column];
    }

    return sum == s_magicSum;
}

template <size_t dimension>
void WorkerThread<dimension>::lastDiag()
{
    for (unsigned i = 0; i < s_square; ++i)
    {
        if (m_usedNumbers[i])
        {
            continue;
        }
        m_usedNumbers[i] = true;
        m_content[s_limit][0] = i + 1;
        if (checkSecDiag() && checkColumn(0))
        {
            backtrackLastRow (1);
        }
        m_usedNumbers[i] = false;
    }
}

template <size_t dimension>
void WorkerThread<dimension>::backtrackLastRow(unsigned i)
{
    for (unsigned j = 0; j < s_square; ++j)
    {
        if (m_usedNumbers[j])
        {
            continue;
        }
        m_usedNumbers[j] = true;
        m_content[s_limit][i] = j + 1;
        if (!checkColumn(i))
        {
            m_usedNumbers[j] = false;
            continue;
        }
        if (i == s_diag_limit)
        {
            if (checkRow(s_limit))
            {
                ++m_numberOfSolutions;
                m_usedNumbers[j] = false;
                continue;
            }
            else
            {
                m_usedNumbers[j] = false;
                continue;
            }
        }
        else
        {
            backtrackLastRow(i + 1);
            m_usedNumbers[j] = false;
            continue;
        }
    }
}

template <size_t dimension>
void WorkerThread<dimension>::createMainDiag(unsigned i, unsigned sum)
{
    unsigned s = 0;
    for (unsigned j = 0; j < s_square; ++j)
    {
        if (m_usedNumbers[j])
        {
            continue;
        }
        m_usedNumbers[j] = true;
        m_content[i][i] = j + 1;
        if (i == s_limit)
        {
            if (checkMainDiag())
            {
                backtrack(0, 0, 0);
            }
            m_usedNumbers[j] = false;
            continue;
        }
        else
        {
            if ((s = sum + j + 1) > s_magicSum)
            {
                m_usedNumbers[j] = false;
                continue;
            }
            createMainDiag(i + 1, s);
            m_usedNumbers[j] = false;
        }
    }
}

template <size_t dimension>
std::thread WorkerThread<dimension>::work (WorkerThread<dimension> * worker)
{
    return std::thread([](WorkerThread<dimension> * t){t->solve();}, worker);
}

#endif // WORKERTHREAD_HPP
