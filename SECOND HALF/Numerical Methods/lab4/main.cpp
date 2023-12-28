#include <iostream>
#include "NonlinearEquations.h"

using namespace Math;

void m()
{
    NonlinearEquations<double> data;
    switch (data.getMethod())
    {
    case NonlinearEquations<double>::METHOD::NEWTON:  // ����� ������� (+ ����� ��������)
        data.Newton();
        break;
    case NonlinearEquations<double>::METHOD::STEEPESTDESCENT: // ����� ������������� ������
        data.SteepestDescent();
        break;
    }

}