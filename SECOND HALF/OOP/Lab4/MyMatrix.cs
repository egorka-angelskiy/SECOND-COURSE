using MMatrix;
using System.Collections;

namespace Math
{
    public class Matrix : IEnumerable, IFormattable, ICloneable
    {
        Random rnd = new Random();


        public static int Counter_id;
        public int Id { get; private set; }
        public int ROWS { get; private set; }
        public int COLUMNS { get; private set; }
        private double[] Array = null;

        public Matrix() : this(3, 3) { Id = Counter_id++; }
        public Matrix(int Rows, int Columns, double[,] array)
        {
            Id = Counter_id++;

            if (Rows < 0 || Columns < 0)
            {
                throw new ArgumentOutOfRangeException($"������ �����������: {Rows}", $"������������ ���������� ����� �������: {Id}");
                throw new ArgumentOutOfRangeException($"������ �����������: {Columns}", $"������������ ���������� �������� �������: {Id}");
            }

            ROWS = Rows;
            COLUMNS = Columns;
            Array = new double[ROWS * COLUMNS];
            for (int i = 0; i < ROWS; ++i)
            {
                for (int j = 0; j < COLUMNS; ++j)
                {
                    Array[i * COLUMNS + j] = array[i, j];
                }
            }    
        }

        public Matrix(int Rows) : this(Rows, Rows, new double[Rows, Rows]) { }

        public Matrix(int Rows, double[,] array) : this(Rows, Rows, array) { }
        public Matrix(int Rows, int Columns, double[] array)
        {
            Id = Counter_id++;

            if (Rows <= 0 || Columns <= 0 || array.Length != Rows * Columns)
            {
                throw new ArgumentOutOfRangeException($"������ �����������: {Rows}", $"������������ ���������� ����� �������: {Id}");
                throw new ArgumentOutOfRangeException($"������ �����������: {Columns}", $"������������ ���������� �������� �������: {Id}");
                throw new ArgumentOutOfRangeException($"������ ����� ������� {array.Length}", $"������������ ���������� ��������� ������� ������� �{Id}: {Rows}x{Columns}");
            }

            ROWS = Rows;
            COLUMNS = Columns;
            Array = new double[ROWS * COLUMNS];
            for (int i = 0; i < ROWS * COLUMNS; ++i)
            {
                Array[i] = array[i];
            }
        }

        public Matrix(int Rows, int Columns) : this(Rows, Columns, new double[Rows * Columns])
        {
            for (int i = 0; i < ROWS; ++i)
            {
                for (int j = 0; j < COLUMNS; j++)
                Array[i * COLUMNS + j] = rnd.Next(-15, 15);
            }
        }

        public Matrix(Matrix copy) : this(copy.ROWS, copy.COLUMNS, copy.Array) { }

        public static bool Addition(Matrix A, Matrix B)
        {
            return (A.ROWS == B.ROWS && A.COLUMNS == B.COLUMNS);
        }

        public static bool Multiplication(Matrix A, Matrix B)
        {
            return (A.ROWS == B.COLUMNS);
        }

        public double Maxim()
        {
            double maxim = Array[0];
            for (int i = 0; i < ROWS * COLUMNS; ++ i)
            {
                if (maxim < Array[i])
                {
                    maxim = Array[i];
                }
            }
            return maxim;
        }

        public double Minim()
        {
            double minim = Array[0];
            for (int i = 0; i < ROWS * COLUMNS; ++i)
            {
                if (minim < Array[i])
                {
                    minim = Array[i];
                }
            }
            return minim;
        }

        public static Matrix operator +(Matrix A, Matrix B)
        {
            if (!Addition(A, B))
            {
                throw new ArgumentException($"�������� �������� � ������� ��������� ����������. 1: {A.Id}, 2: {B.Id}");
            }

            Matrix result = new Matrix(A);
            for (int i = 0; i < B.ROWS * B.COLUMNS; ++i)
            {
                result.Array[i] += B.Array[i];
            }

            return result;
        }

        public static Matrix operator -(Matrix A, Matrix B)
        {
            if (!Addition(A, B))
            {
                throw new ArgumentException($"�������� �������� � ������� ��������� ����������. 1: {A.Id}, 2: {B.Id}");
            }

            Matrix result = new Matrix(A);
            for (int i = 0; i < B.ROWS * B.COLUMNS; ++i)
            {
                result.Array[i] -= B.Array[i];
            }

            return result;
        }

        public static Matrix operator *(Matrix A, Matrix B)
        {
            if (!Multiplication(A, B))
            {
                throw new ArgumentException($"�������� ��������� � ������� ��������� ����������. 1: {A.Id}, 2: {B.Id}");
            }


            double temp;
            double[] matrix = new double[A.ROWS * B.COLUMNS];
            for (int i = 0; i < A.ROWS; ++i)
                for (int j = 0; j < B.COLUMNS; ++j)
                {
                    temp = 0;
                    for (int k = 0; k < B.ROWS; ++k)
                    {
                        temp += A.Array[i * A.COLUMNS + k] * B.Array[k * B.COLUMNS + j];
                    }
                        
                    matrix[i * B.COLUMNS + j] = temp;
                }
            return new Matrix(A.ROWS, B.COLUMNS, matrix);
        }

        public static Matrix operator *(Matrix A, double scalar)
        {
            Matrix result = new Matrix(A);
            for (int i = 0; i < A.ROWS * A.COLUMNS; i++)
            {
                result.Array[i] *= scalar;
            }
                
            return result;
        }

        public double this[int i, int j]
        {
            get
            {
                if (i < 0 || i > ROWS || j < 0 || j > COLUMNS)
                {
                    throw new ArgumentOutOfRangeException("����� �� ������� �������.");
                }
                    
                return Array[i * COLUMNS + j];
            }
            set
            {
                if (i < 0 || i > ROWS || j < 0 || j > COLUMNS)
                {
                    throw new ArgumentOutOfRangeException("����� �� ������� �������.");
                }
                    
                Array[i * COLUMNS + j] = value;
            }
        }


        //public override string ToString()
        //{
        //    string output = "";
        //    for (int i = 0; i < ROWS; i++)
        //    {
        //        output += "[\t";
        //        for (int j = 0; j < COLUMNS; j++)
        //        {
        //            output += Array[i * COLUMNS + j].ToString() + '\t';
        //        }
        //        output += "]\n";
        //    }

        //    return output;
        //}

        object ICloneable.Clone()
        {
            return Clone();
        }

        public Matrix Clone()
        {
            Matrix toOut = new Matrix(this.ROWS, this.COLUMNS, this.Array);

            return toOut;
        }

        string IFormattable.ToString(string format, IFormatProvider formatProvider)
        {
            string output = "";
            for (int i = 0; i < ROWS; i++)
            {
                output += "[\t";
                for (int j = 0; j < COLUMNS; j++)
                {
                    output += Array[i * COLUMNS + j].ToString() + '\t';
                }
                output += "]\n";
            }

            return output;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return Array.GetEnumerator();
        }
    }
}