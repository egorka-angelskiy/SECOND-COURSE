#pragma once

#include <iomanip>
#include <iostream>
#include "PolStr.h"
#define MAX_ITER 100000

class Methods
{
	public:
        Methods(const char* pstr, double a, double b, double eps, std::string expr, char method = '1') :
            _pstr{ pstr }, _a(a), _b(b), _eps(eps), _expr{ expr }, _method(method), _count(0), _eps_new(0), _res(0){}

        ~Methods(){}

        std::string getType() const
        {
            switch (_method)
            {
            case '1':
                return "����� ���������";
            case '2':
                return "����� ����";
            case '3':
                return "����� �������� �������";
            case '4':
                return "��������������� �����";
            case '5':
                return "����� �������";
            case '6':
                return "����� ��������";
            default:
                return "�� ���������";
            }
        }

        int getCount()
        {
            return _count;
        }

        friend std::ostream& operator<<(std::ostream& out, const Methods& expr)
        {
            std::streamsize precision = std::cout.precision();
            out << expr.getType() << ": \n"
                << expr._expr
                << "\nx' = " << std::setprecision(precision) << expr._res << "\n"
                << "f(x') = " << EvalPolStr(expr._pstr, expr._res, 0)
                << "\n������� �����������: " << expr._eps
                << "\n���������� ����������� ����������: " << expr._eps_new << std::endl;
            return out;
        }

        void setResult(char choice)
        {
            _method = choice;
            switch (_method)
            {
            case '1':
                setDichotomyMethod();
                break;
            case '2':
                setChordMethod();
                break;
            case '3':
                setGoldenSectionMethod();
                break;
            case '4':
                setCombinedMethod();
                break;
            case '5':
                setNewtonMethod();
                break;
            case '6':
                setIterationMethod();
                break;
            default:
                throw std::invalid_argument("�� ������ ���������� ����� �������");
            }
        }

        //����� ��������� - 1
        double setDichotomyMethod()
        {
            double a = _a, b = _b, eps = _eps, c;
            double f_a = EvalPolStr(_pstr, a, 0), f_b, f_c;
            int count = 0;
            while ((b - a) / 2 >= eps && count < MAX_ITER) // �������� ���������� �� �������
            {
                c = (a + b) / 2;
                f_c = EvalPolStr(_pstr, c, 0);
                if (f_a * f_c <= 0)
                {
                    b = c;
                    f_b = f_c;
                }
                else
                {
                    a = c;
                    f_a = f_c;
                }
                count++;
            }
            _count = count;
            _res = (a + b) / 2;
            _eps_new = (b - a) / 2;
            return _res;
        }

        //����� ���� - 2
        double setChordMethod()
        {
            double a = _a, b = _b, eps = _eps;
            double c;
            double x; // ��������� �����������
            if (EvalPolStr(_pstr, a, 0) * EvalPolStr(_pstr, a, 2) > 0)
                x = a;
            else x = b;
            double f_a = EvalPolStr(_pstr, a, 0),
                f_b = EvalPolStr(_pstr, b, 0),
                f_c;
            int count = 0;
            do {
                c = a - f_a * (b - a)
                    / (f_b - f_a);
                f_c = EvalPolStr(_pstr, c, 0);
                if (f_a * f_c <= 0)
                {
                    b = c;
                    f_b = f_c;
                }
                else
                {
                    a = c;
                    f_a = f_c;
                }
                x = x - EvalPolStr(_pstr, x, 0) / EvalPolStr(_pstr, x, 1);
                count++;
            } while (abs(c - x) > eps && abs(EvalPolStr(_pstr, c, 0)) > eps && count < MAX_ITER);
            _count = count;
            _res = c;
            _eps_new = abs(c - x);
            return c;
        }

        //����� �������� ������� - 3
        double setGoldenSectionMethod()
        {
            double a = _a, b = _b, eps = _eps;
            double c, d, h = (sqrt(5) + 1) / 2;
            double f_a = EvalPolStr(_pstr, a, 0),
                f_d;
            int count = 0;
            while ((b - a) / 2 > eps && abs(EvalPolStr(_pstr, (a + b) / 2, 0)) > eps && count < MAX_ITER)
            {
                d = a + (b - a) / h; //����� �������� ������� - 3
                c = a + b - d; // c - a = b - d / ��� ����� ����� �������
                f_d = EvalPolStr(_pstr, d, 0);

                if (f_a * f_d <= 0)
                    b = d;
                else
                {
                    a = c;
                    f_a = EvalPolStr(_pstr, c, 0);
                }
                count++;
            }
            _count = count;
            _res = (a + b) / 2;
            _eps_new = (b - a) / 2;
            return _res;
        }


        //��������������� ����� - 4
        double setCombinedMethod()
        {
            double a = _a, b = _b, eps = _eps;
            double f_a0 = EvalPolStr(_pstr, a, 0);
            double c;
            int count = 0;
            do {
                // ��������� �����������
                c = a - f_a0 * (b - a)
                    / (EvalPolStr(_pstr, b, 0) - f_a0);
                // ���� f(a)f''(a) > 0
                if (f_a0 * EvalPolStr(_pstr, a, 2) > 0)
                {
                    // ����� � ������������ �� ������ �������(�����������)
                    a = a - f_a0 / EvalPolStr(_pstr, a, 1);
                    f_a0 = EvalPolStr(_pstr, a, 0);
                    // � ����� b ������������ �� ������ ����
                    b = c;
                }

                // �.�. ���� f(b)f''(b) > 0
                else
                {
                    // ����� � ������������ �� ������ ����
                    a = c;
                    // � ����� b ������������ �� ������ �������
                    b -= EvalPolStr(_pstr, b, 0) / EvalPolStr(_pstr, b, 1);
                }
                count++;
            } while (/*(b - a) / 2 > eps &&*/ abs(EvalPolStr(_pstr, (a + b) / 2, 0)) > eps && count < MAX_ITER);
            _count = count;
            _res = (a + b) / 2;
            _eps_new = (b - a) / 2;
            return _res;
        }

        //����� ������� - 5
        double setNewtonMethod()
        {
            double a = _a, b = _b, eps = _eps;

            double x0; // ��������� �����������
            if (EvalPolStr(_pstr, a, 0) * EvalPolStr(_pstr, a, 2) > 0)
                x0 = a;
            else x0 = b;
            double x1 = x0;
            int count = 0;
            do {
                x1 = x1 - EvalPolStr(_pstr, x1, 0) / EvalPolStr(_pstr, x1, 1);
                if (abs(x1 - x0) < eps) break;
                x0 = x1;
                count++;
            } while (count < MAX_ITER);
            _count = count;
            _res = x1;
            _eps_new = abs(x1 - x0);
            return x1;
        }




        double maxf(double a, double b)
        {
            const double goldenRatio = (1 + sqrt(5)) / 2; // "�������" �����
            double x1, x2; // �����, ������� ������� ������� � ��������� �������� �������
            while (fabs(b - a) > _eps)
            {
                x1 = b - (b - a) / goldenRatio;
                x2 = a + (b - a) / goldenRatio;
                if (EvalPolStr(_pstr, x1, 1) <= EvalPolStr(_pstr, x2, 1))
                    a = x1;
                else
                    b = x2;
            }
            return (a + b) / 2;
        }


        //����� �������� - 6
        double setIterationMethod()
        {
            double a = _a, b = _b, eps = _eps;
            double x0 = a, x1 = b;
            double f_x0 = EvalPolStr(_pstr, x0, 1), f_x1 = EvalPolStr(_pstr, x1, 1);

            double max = maxf(a, b);
            int count = 0;
            do {
                x1 = x1 - EvalPolStr(_pstr, x1, 0) / max;
                if (abs(x0 - x1) <= eps) break;
                x0 = x1;
                count++;
            } while (count < MAX_ITER);
            _count = count;
            _eps_new = abs(x0 - x1);
            _res = x0 - EvalPolStr(_pstr, x0, 0) / max;
            return _res;
        }

	private:
        double _a, _b, _eps;
        const char* _pstr;
        char _method;
        double _res;
        double _eps_new;
        std::string _expr;
        int _count;

};