using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SudokuSolver
{
    class Program
    {
        static void Main(string[] args)
        {
            IEnumerable<int>[] rows = [[0,0,0,0,7,0,4,0,0]];
            Console.ReadLine();
        }
    }
    public class Sudoku
    {
        public int _size;
        SudokuMatrix<int> _workingset = new SudokuMatrix<int>();
        SudokuMatrix<int[]> _possiblitityset = new SudokuMatrix<int[]>();
        public int Size
        {
            get
            {
                return _size;
            }
            set
            {
                _size = value;
            }
        }
        public SudokuMatrix<int> Matrix
        {
            get
            {
                return _workingset;
            }
        }
        public Sudoku(int Size, IEnumerable<int>[] rows)
            : base()
        {
            if (Math.Sqrt(Size) % 1 != 0)
            {
                throw new System.ArgumentException("Size is not a perfect square, Sudoku not possible!");
            }
            else
            {
                _size = Size;
                _workingset.Create(rows);
            }

        }
    }
    class SudokuMatrix<T>
    {
        List<List<T>> matrix;
        public void Create(IEnumerable<T>[] rows)
        {
            for (int i = 0; i < rows.Length; i++)
            {
                if (rows.Length != rows[i].Count())
                {
                    throw new System.ArgumentException("Incorrect row length!");
                }
                else
                {
                    List<T> inputRow = new List<T>(rows[i]);
                    matrix.Add(inputRow);
                }
            }
        }
        public T this[int x, int y]
        {
            get { return matrix[y][x]; }
            set { matrix[y][x] = value; }
        }
        public void Print()
        {
            for (int i = 0; i < matrix.Count(); i++)
            {

            }
        }
    }
}