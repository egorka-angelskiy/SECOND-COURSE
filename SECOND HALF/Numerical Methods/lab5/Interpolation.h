
#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "Vector.h"
#include "Polynomial.h"
#include "PolStr.h"

#define EPS 1e-5

namespace Math
{
    std::streambuf* redirectInput(std::ifstream* fin = NULL);
    std::streambuf* redirectOutput(std::ofstream* fout = NULL);
    template<class T> inline T factorial(T number);
    char getSymbol(std::initializer_list<char> list,
        std::string notification_message = "",
        std::string error_message = "������������ ��������, ���������� ��� ���.\n-> ");
    double getDouble(double min = -DBL_MAX,
        double max = DBL_MAX,
        std::string notification_message = "",
        std::string error_message = "������������ ��������, ���������� ��� ���.\n-> ");
    template<class T> T* getGridX(T* temp_array, int size,
        std::string notification_message = "",
        std::string error_message = "������������ ��������, ���������� ��� ���.");

    template<class T>
    class Interpolation
    {
    private:
        std::streambuf* _original_cin;
        std::streambuf* _original_cout;
        char _method;  // ��������� ����� ���������������� 
        // (1 � ������� �������,
        //  2 � ������� ��������);
        unsigned _k;        //  ������� ����������� 
        // (0 � ����������� ��� �������, 
        //  1 � ��� ������ �����������,
        //  2 � ��� ������ �����������)
        unsigned _n;        // ������� ��������;
        char _s;       // ������, �������� ��� �������� ����� 			 	   
        // (u � uniform grid     � ����������� �����,
        //  n � non-uniform grid � ������������� �����);
        T _a;
        T _b;          // ������� ������� (��� ����������� �����);
        Vector<T> _x0; // ���� �����(���� ��� �������������);
        Vector<T> _y0; // �������� ������� � ����� �����;
        unsigned _m;        // ���������� ���������� � �������������� �����
        // (�.�.���������� ����� � m + 1,
        //  ��� ������� ��� ���������� � ������ �������� �����);
        Vector<T> _res_x; // ���� �������������� �����;
        char _t;          // ������, ����������, �������� ��� ��� ������������� ��������� ��� ������� f(x)
        // (y - ������������� ��������� ��������,
        //  n - ������������� ��������� ����������);
        char* _f;   // ������������� ��������� ��� ������� (���� ��� ��������).
    public:

        Interpolation()
            : _original_cin{ std::cin.rdbuf() }, _original_cout{ std::cout.rdbuf() },
            _method(1), _k(0), _n(-1), _s('u'), _a(0), _b(0),
            _x0(Vector<T>()), _y0(Vector<T>()), _m(0),
            _res_x(Vector<T>()), _t('n'), _f(NULL)
        {
        }

        // ��������� ������ �����
        std::ifstream* setInputDevice(char input_method)
        {
            std::ifstream* fin = NULL;
            std::ofstream* fout = NULL;
            switch (input_method)
            {
            case '1': return NULL;
            case '2':
            {
                std::string filename;
                std::cout << "\n\t������� ��� �������� �����:\n-> ";
                getline(std::cin, filename);
                fin = new std::ifstream(filename);
                // ��������� ������ ����� ����� � �������������� ��� �� ���������������� ����
                _original_cin = redirectInput(fin);
                if (!_original_cin)  return NULL;

                std::cout << "\n\t������� ��� ��������� �����:\n-> ";
                getline(std::cin, filename);
                fout = new std::ofstream(filename);
                // ��������� ������ ����� ������ � �������������� ��� �� ���������������� ����
                _original_cout = redirectOutput(fout);
                if (!_original_cout)  return NULL;
                break;
            }
            case '3': case '4':
                if (input_method == '3')
                    fin = new std::ifstream("input_uniform_grid.txt");
                else
                    fin = new std::ifstream("input_non-uniform_grid.txt");
                // ��������� ������ ����� ����� � �������������� ��� �� ���� input_non-uniform_grid.txt 
                _original_cin = redirectInput(fin);
                if (!_original_cin) return NULL;

                fout = new std::ofstream("output.txt");
                // ��������� ������ ����� ������ � �������������� ��� �� ���� output.txt
                _original_cout = redirectOutput(fout);
                if (!_original_cout)  return NULL;
                break;
            default:
                throw std::invalid_argument("\n\t\t��� ����������� ������ ����� ������...\n");
            }
            return fin;
        }

        // ���������� ������ �� �������� ������ �����
        void inputData(std::ifstream* in)
        {
            if (!in)
            {
                _method = getSymbol({ '1', '2' }, "\n\t������� ��������� ����� ����������������:"
                    "\n\t1 � ������� �������;"
                    "\n\t2 � ������� ��������.\n-> ");
                _k = getSymbol({ '0','1','2' }, "\n\t������� �����������:"
                    "\n\t0 � ����������� ��� �������;"
                    "\n\t1 � ��� ������ �����������;"
                    "\n\t2 � ��� ������ �����������.\n-> ") - '0';
                _n = static_cast<unsigned>(getDouble(0, INT_MAX, "\n\t������� ������� ��������:\n-> "));
                _s = getSymbol({ 'u','n' }, "\n\t������� ������, �������� ��� �������� �����:"
                    "\n\tu � uniform grid     � ����������� �����;"
                    "\n\tn � non-uniform grid � ������������� �����).\n-> ");

                T* temp_array = new T[_n + 1];
                if (_s == 'u')
                {
                    _a = getDouble(-INT_MAX, INT_MAX, "\n\t������� ����� ������� ���������������� �������, �� ������� ����� ����������������� �������:\n-> ");
                    _b = getDouble(_a, INT_MAX, "\n\t������� ������ ������� ���������������� �������, �� ������� ����� ����������������� �������:\n-> ",
                        "\n\t������ ������� ��������� ������ ���� ������ �����.");
                    std::cout << "\n\t������� �������� ������� � ����� ���������������� �����:\n";

                    T step = (_b - _a) / _n;
                    unsigned index = 0;
                    for (T x_i = _a; x_i <= _b; x_i += step, index++)
                        temp_array[index] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << x_i << "]: ").str());
                    _y0 = Vector<T>(_n + 1, false, temp_array);
                    delete[] temp_array;
                }
                else
                {
                    _x0 = Vector<T>(_n + 1,
                        false, getGridX(temp_array, _n + 1,
                            "\n\t������� �������� ����� ���������������� �����:\n",
                            "\n\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������."));
                    delete[] temp_array;
                    std::cout << "\n\t������� �������� ������� � ����� ���������������� �����:\n";

                    for (unsigned i = 0; i <= _n; i++)
                        temp_array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << _x0[i] << "]: ").str());
                    _y0 = Vector<T>(_n + 1, false, temp_array);
                    delete[] temp_array;
                }


                _m = static_cast<unsigned>(getDouble(0, INT_MAX, "\n\t������� ���������� ���������� � �������������� �����:\n-> "));
                temp_array = new T[_m + 1];
                _res_x = Vector<T>(_m + 1,
                    false, getGridX(temp_array, _m + 1,
                        "\n\t������� �������� ����� �������������� ���������������� �����:\n",
                        "\n\t\t\t�������� ����� ������ ���� ������ �� �����������. ������� ������ ��������.\n"));
                delete[] temp_array;

                _t = getSymbol({ 'y', 'n' }, "\n\t�������� �� ������������� ��������� ��� ������� f(x)?"
                    "\n\ty � ��;"
                    "\n\tn � ���\n-> ");
                if (_t == 'y')
                {
                    std::string F;
                    char choice = 'y';
                    while (choice == 'y')
                    {
                        std::cout << "\n\t������� ��������� ��� �������:\n-> ";
                        getline(std::cin, F);
                        if (!F.empty())
                        {
                            _f = CreatePolStr(F.c_str(), 0);
                            if (GetError() != ERR_OK)
                            {
                                std::cerr << "\n\t������ ��������� �� ��������������.";
                                choice = getSymbol({ 'y','n' }, "\n\t����������� ��� ���? (y/n)\n-> ");
                            }
                            else choice = 'n';
                        }
                        else
                        {
                            std::cerr << "\n\t������ ���������� ������ ������.";
                            choice = getSymbol({ 'y','n' }, "\n\t����������� ��� ���? (y/n)\n-> ");
                        }
                    }

                }
                if (in)
                {
                    std::cin.rdbuf(_original_cin); // ���������� ����� �� ������������ ������������������ �����
                    in->close();
                }
            }
            else
            {
                std::cin >> _method >> _k >> _n >> _s;
                T* temp_array = new T[_n + 1];
                if (_s == 'u')
                    std::cin >> _a >> _b;
                else
                {
                    for (unsigned i = 0; i <= _n; i++)
                        std::cin >> temp_array[i];
                    _x0 = Vector<T>(_n + 1, false, temp_array);
                    delete[] temp_array;
                }
                temp_array = new T[_n + 1];
                for (unsigned i = 0; i <= _n; i++)
                    std::cin >> temp_array[i];
                _y0 = Vector<T>(_n + 1, false, temp_array);
                delete[] temp_array;

                std::cin >> _m;
                temp_array = new T[_m + 1];
                for (unsigned i = 0; i <= _m; i++)
                    std::cin >> temp_array[i];
                _res_x = Vector<T>(_m + 1, false, temp_array);
                delete[] temp_array;

                std::cin >> _t;
                if (_t == 'y')
                {
                    std::cin.seekg(2, std::ios_base::cur);
                    std::string F;
                    getline(std::cin, F);
                    _f = CreatePolStr(F.c_str(), 0);
                    if (GetError() != ERR_OK)
                        std::cout << "\n\t����������� ������ ������������� ������� (��� �� �������� ���������). \n";
                }
            }
        }
        ~Interpolation()
        {
            if (_original_cout)
                std::cout.rdbuf(_original_cout); // ���������� �� ������������ ����� � ����������
            if (_f) { delete[] _f; _f = NULL; }
        }

        char getMethod() const { return _method; }
        unsigned getDerivative() const { return _k; }
        unsigned getPolOrder() const { return _n; }
        char getGridType() const { return _s; }
        Vector<T> getSourceGrid() const
        {
            if (_s == 'u')
            {
                T step = (_b - _a) / _n;
                Vector<T> x0(_n + 1, false);
                unsigned index = 0;
                for (T x_i = _a; x_i <= _b; x_i += step, index++)
                    x0[index] = x_i;
                return std::move(x0);
            }
            else return _x0;
        }
        Vector<T> getValueGrid() const { return _y0; }
        Vector<T> getResultGrid() const { return _res_x; }
        char* getOrigAnalytic() const { return _f; }


        Vector<Vector<T>> getDividedDifferences()
        {
            Vector<Vector<T>> dividedDifferences(_n + 1, true); // ������-������� 
            dividedDifferences[0] = Vector<T>(_n + 1, false);   // �������� ������-������ (y0,...,yn)
            for (unsigned i = 0; i <= _n; i++)
                dividedDifferences[0][i] = _y0[i];

            for (unsigned i = 1; i <= _n; i++)
            {
                dividedDifferences[i] = Vector<T>(_n + 1 - i, false); // �������� ������-������ ����������/�������� ��������� [x_i,...,x_(i+k)]/[y_i,...,y_(i+k)]
                for (unsigned j = 0; j <= _n - i; j++)
                    if (_s == 'u') // ����� �����������
                        dividedDifferences[i][j] = (dividedDifferences[i - 1][j + 1] - dividedDifferences[i - 1][j]);
                    else           // ����� �������������
                        dividedDifferences[i][j] = (dividedDifferences[i - 1][j + 1] - dividedDifferences[i - 1][j])
                        / (_x0[i + j] - _x0[j]);
            }
            return dividedDifferences;
        }

        Polynomial<T> getNewtonInterPol(unsigned deg = _k)
        {
            Vector<Vector<T>> dividedDifferences(getDividedDifferences());
            switch (deg)
            {
            case 0:
                return NewtonInterPol(dividedDifferences);
                break;
            case 1:
                return Dx1NewtonInterPol(dividedDifferences);
                break;
            case 2:
                return Dx2NewtonInterPol(dividedDifferences);
                break;
            default: throw std::logic_error("�������������� ������� �����������");
            }
            return  Polynomial<T>();
        }

        Polynomial<T> getLagrangeInterPol(unsigned deg = _k)
        {
            switch (deg)
            {
            case 0:
                return LagrangeInterPol();
                break;
            case 1:
                return Dx1LagrangeInterPol();
                break;
            case 2:
                return Dx2LagrangeInterPol();
                break;
            default: throw std::logic_error("�������������� ������� �����������");
            }
            return  Polynomial<T>();
        }

        void checkSourceGrid(Polynomial<T> pol)
        {
            bool success = true;
            if (_s == 'u') // uniform grid     � ����������� �����
            {
                unsigned index = 0;

                for (T q = _a, step = (_b - _a) / _n; q <= _b; q += step, index++)
                {
                    std::cout << "\tP(" << q << ") = " << pol(q) << "\n";
                    if (abs(pol(q) - _y0[index]) > EPS)
                    {
                        success = false;
                        std::cout << "\t f(" << index << ") = " << _y0[index]
                            << "\n\t������� ������������ �� �����������. �����������: " << abs(pol(q) - _y0[index]) << "\n";
                    }
                }
            }
            else           // non-uniform grid � ������������� �����
            {
                for (unsigned i = 0; i <= _n; i++)
                {
                    std::cout << "\tP(" << _x0[(unsigned)i] << ") = " << pol(_x0[(unsigned)i]) << "\n";
                    if (abs(pol(_x0[(unsigned)i]) - _y0[i]) > EPS)
                    {
                        success = false;
                        std::cout << "\t f(" << _x0[(unsigned)i] << ") = " << _y0[i]
                            << "\n\t������� ������������ �� �����������. �����������: " << abs(pol(_x0[(unsigned)i]) - _y0[i]) << "\n";
                    }
                }
            }
            if (success) std::cout << "\n\t������ ���������������� ������� � ����� �������� ����� ��������� � �������� �������� �����, ������� ����������."
                << "\n������� ������������ ���������.";
        }

        void checkResGrid(Polynomial<T> pol)
        {
            for (unsigned i = 0; i <= _m; i++)
            {
                std::cout << "\tP(" << _res_x[(unsigned)i] << ") = " << pol(_res_x[(unsigned)i]) << "\n";
                if (_f)
                {
                    T res_f = EvalPolStr(_f, _res_x[(unsigned)i], _k);
                    std::cout << "\tf(" << _res_x[(unsigned)i] << ") = " << res_f
                        << "\n\t�����������: " << abs(pol(_res_x[(unsigned)i]) - res_f) << "\n";
                }
            }
        }

        friend std::ostream& operator<<(std::ostream& out, Interpolation& data)
        {
            std::streamsize precision = std::cout.precision();
            std::streamsize size = std::cout.width();
            std::cout << std::setprecision(precision);

            out << "\n\t�������� ������� �������� ������������ ���������������� �������: " << data.getPolOrder()
                << "\n\t��� �������� �����: ";
            if (data.getGridType() == 'u')
                out << "����������� �����.\n";
            else out << "������������� �����.\n";
            out << "��������� �����: " << "\t\t\t\t\t\t" << std::setw(size) << data.getSourceGrid()
                << "��������������� �������� ������ �����: " << "\t" << std::setw(size) << data.getValueGrid()
                << "��������� ������� ����������� ��������������� �������: " << data.getDerivative();

            switch (data.getMethod())
            {
            case '1':
                out << "\n������� ������� � " << data.getDerivative() << " �������� �����������:\t"
                    << std::setw(size) << data.getNewtonInterPol(data.getDerivative())
                    << "\n������ �������� ��������������� ������� � ��������� �������� ����� � �������� �����: \n";
                data.checkSourceGrid(data.getNewtonInterPol(0));
                out << "\n�������� �������� ��������������� ������� ���������� ������� ����������� � �������������� �����:\n";
                data.checkResGrid(data.getNewtonInterPol(data.getDerivative()));

                break;
            case '2':
                out << "\n������� �������� � " << data.getDerivative() << " �������� �����������:\t"
                    << std::setw(size) << data.getLagrangeInterPol(data.getDerivative())
                    << "\n������ �������� ��������������� ������� � ��������� �������� ����� � �������� �����: \n";
                data.checkSourceGrid(data.getLagrangeInterPol(0));
                out << "\n�������� �������� ��������������� ������� ���������� ������� ����������� � �������������� �����:\n";
                data.checkResGrid(data.getLagrangeInterPol(data.getDerivative()));
                break;
            }
            return out;
        }

    private:
        // ��������� ����������������� �������� ������� � ������������� ����
        Polynomial<T> NewtonInterPol(Vector<Vector<T>>& dividedDifferences)
        {
            Polynomial<T> P;
            for (int i = 0; i <= (int)_n; i++)
            {
                Polynomial<T> mult((T)1);
                for (int j = 0; j <= i - 1; j++)
                {
                    if (_s == 'u')
                        mult *= Polynomial<T>({ -(T)j, 1 });
                    else
                        mult *= Polynomial<T>({ -_x0[j], 1 });

                }
                if (_s == 'u')
                    P += dividedDifferences[i][0] * mult / factorial(i);
                else
                    P += dividedDifferences[i][0] * mult;
            }
            return P;
        }

        // ��������� ����������� ������� ������� ����������������� �������� ������� � ������������� ����
        Polynomial<T> Dx1NewtonInterPol(Vector<Vector<T>>& dividedDifferences)
        {
            Polynomial<T> P_first;
            for (unsigned i = 1; i <= _n; i++)
            {
                Polynomial<T> temp_sum;
                for (unsigned j = 0; j <= i - 1; j++)
                {
                    Polynomial<T> mult((T)1);
                    for (int k = 0; k <= i - 1; k++)
                        if (k != j)
                            if (_s == 'u')
                                mult *= Polynomial<T>({ -(T)k, 1 });
                            else
                                mult *= Polynomial<T>({ -_x0[k], 1 });
                    temp_sum += mult;
                }
                if (_s == 'u')
                    P_first += temp_sum * dividedDifferences[i][0] / factorial(i);
                else
                    P_first += temp_sum * dividedDifferences[i][0];
            }
            return P_first;
        }

        // ��������� ����������� ������� ������� ����������������� �������� ������� � ������������� ����
        Polynomial<T> Dx2NewtonInterPol(Vector<Vector<T>>& dividedDifferences)
        {
            Polynomial<T> P_second;
            for (unsigned i = 2; i <= _n; i++)
            {
                Polynomial<T> temp_sum1;
                for (unsigned j = 0; j <= i - 1; j++)
                {
                    Polynomial<T> temp_sum2;
                    for (unsigned k = 0; k <= i - 1; k++)
                        if (k != j)
                        {
                            Polynomial<T> mult((T)1);
                            for (int l = 0; l <= i - 1; l++)
                                if (l != j && l != k)
                                    if (_s == 'u')
                                        mult *= Polynomial<T>({ -(T)l, 1 });
                                    else
                                        mult *= Polynomial<T>({ -_x0[l], 1 });
                            temp_sum2 += mult;
                        }
                    temp_sum1 += temp_sum2;
                }
                if (_s == 'u')
                    P_second += temp_sum1 * dividedDifferences[i][0] / factorial(i);
                else
                    P_second += temp_sum1 * dividedDifferences[i][0];

            }
            return P_second;
        }

        // ��������� ����������������� �������� �������� � ������������� ����
        Polynomial<T> LagrangeInterPol()
        {
            T h;
            if (_s == 'u') h = (_b - _a) / _n;
            Polynomial<T> P;
            for (int i = 0; i <= _n; i++)
            {
                Polynomial<T> mult((T)1);
                for (int j = 0; j <= _n; j++)
                    if (j != i)
                        if (_s == 'u')
                            mult *= Polynomial<T>({ -(T)j, 1 }) / (i - j);
                        else
                            mult *= Polynomial<T>({ -_x0[j], 1 }) / (_x0[i] - _x0[j]);

                P += mult * _y0[i];
            }
            return std::move(P);
        }

        // ��������� ����������� ������� ������� ����������������� �������� �������� � ������������� ����
        Polynomial<T> Dx1LagrangeInterPol()
        {
            T h;
            if (_s == 'u') h = (_b - _a) / _n;
            Polynomial<T> P_first;
            for (int i = 0; i <= _n; i++)
            {
                Polynomial<T> temp_sum;
                for (int j = 0; j <= _n; j++)
                    if (j != i)
                    {
                        Polynomial<T> mult((T)1);
                        for (int k = 0; k <= _n; k++)
                            if (k != i && k != j)
                                if (_s == 'u')
                                    mult *= Polynomial<T>({ -(T)k, 1 }) * h;
                                else
                                    mult *= Polynomial<T>({ -_x0[k], 1 });

                        temp_sum += mult;
                    }
                T mult = (T)1;
                for (int j = 0; j <= _n; j++)
                    if (j != i)
                        if (_s == 'u')
                            mult *= (i - j) * h;
                        else
                            mult *= _x0[i] - _x0[j];

                P_first += temp_sum * _y0[i] / mult;
            }
            return P_first;
        }

        // ��������� ����������� ������� ������� ����������������� �������� �������� � ������������� ����
        Polynomial<T> Dx2LagrangeInterPol()
        {
            T h;
            if (_s == 'u') h = (_b - _a) / _n;
            Polynomial<T> P_second;
            for (int i = 0; i <= _n; i++)
            {
                Polynomial<T> temp_sum1;
                for (int j = 0; j <= _n; j++)
                    if (j != i)
                    {
                        Polynomial<T> temp_sum2;
                        for (int k = 0; k <= _n; k++)
                            if (k != i && k != j)
                            {
                                Polynomial<T> mult((T)1);
                                for (int l = 0; l <= _n; l++)
                                    if (l != i && l != j && l != k)
                                        if (_s == 'u')
                                            mult *= Polynomial<T>({ -(T)l, 1 }) * h;
                                        else
                                            mult *= Polynomial<T>({ -_x0[l], 1 });
                                temp_sum2 += mult;
                            }
                        temp_sum1 += temp_sum2;
                    }
                T mult = (T)1;
                for (int j = 0; j <= _n; j++)
                    if (j != i)
                        if (_s == 'u')
                            mult *= (i - j) * h;
                        else
                            mult *= _x0[i] - _x0[j];
                P_second += temp_sum1 * _y0[i] / mult;
            }
            return P_second;
        }
    };


    std::streambuf* redirectInput(std::ifstream* in)
    {
        std::streambuf* original_cin = std::cin.rdbuf();
        while (!*in)
        {
            std::string filename;
            char choice;
            choice = getSymbol({ '1', '2' },
                "������ ���� �� ����� ���� ������, ���� �� ����������. ����������� ��� ���?\n1) ��\n2) �����\n->");
            if (choice == '1')
            {
                std::cout << "������� ��� �����:\n->";
                getline(std::cin, filename);
            }
            else return NULL;
            in->open(filename);
        }
        //�������������� ����������� ����� ����� �� ���������� ����
        std::cin.rdbuf(in->rdbuf());
        return original_cin;
    }

    std::streambuf* redirectOutput(std::ofstream* out)
    {
        std::streambuf* original_cout = std::cout.rdbuf();
        while (!*out)
        {
            std::string filename;
            char choice;
            choice = getSymbol({ '1', '2' },
                "������ ���� �� ����� ���� ������, ���� �� ����������. ����������� ��� ���?\n1) ��\n2) �����\n->");
            if (choice == '1')
            {
                std::cout << "������� ��� �����:\n->";
                getline(std::cin, filename);
            }
            else return NULL;
            out->open(filename);
        }
        //�������������� ����������� ����� ������ �� ���������� ����
        std::cout.rdbuf(out->rdbuf());
        return original_cout;
    }

    template<class T> inline T factorial(T number)
    {
        T res = 1;
        for (unsigned i = 1; i <= number; i++)
            res *= i;
        return res;
    }

    char getSymbol(std::initializer_list<char> list,
        std::string notification_message,
        std::string error_message)
    {
        char choice;
        std::cout << notification_message;

        bool flag = true;
        do {
            choice = _getche();
            std::cout << std::endl;
            for (auto it = list.begin(); it != list.end(); it++)
                if (it[0] == choice) { flag = false; break; }

            if (flag) std::cerr << error_message;
        } while (flag);
        return choice;
    }

    double getDouble(double min, double max,
        std::string notification_message,
        std::string error_message)
    {
        std::string epsstr;
        double eps;
        do {
            std::cout << notification_message;
            std::cin >> epsstr;

            bool point = false, flag = false;
            auto it = epsstr.begin();
            if (it[0] == '-') it++;
            for (; it != epsstr.end(); it++)
                if (!isdigit(it[0]) && (it[0] != ',' || point))
                {
                    std::cout << error_message;
                    flag = true;
                    break;
                }
                else if (it[0] == ',' && !point) point = true;

            if (flag) continue;
            eps = std::stod(epsstr);
            if (eps <= min || eps >= max)
                std::cout << error_message;
            else { std::cin.ignore(32256, '\n'); break; }
        } while (true);
        return eps;
    }

    template<class T> T* getGridX(T* array, int size,
        std::string notification_message,
        std::string error_message)
    {
        std::cout << notification_message;
        int i = 0;
        array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
        i++;
        do
        {
            array[i] = getDouble(-DBL_MAX, DBL_MAX, (std::stringstream() << "-> [" << i << "]: ").str());
            if (array[i] < array[i - 1]) std::cout << error_message;
            else i++;
        } while (i < size);
        return array;
    }
}
#endif