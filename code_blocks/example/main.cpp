#include <iostream>
#include "sample_rate_converter.hpp"

using namespace std;

int main() {
    cout << "Hello world!" << endl;

    /*  попробуем преобразовать 1 секунду данных с частотой дискретизации 3500 Гц
        в данные с частотой дискретизации 1200 Гц */
    const int FREQ_INPUT_DATA = 3500;
    const int FREQ_OUTPUT_DATA = 1200;

    const double INPUT_DATA_FREQ = 120.0;         // частота сигнала
    const double INPUT_DATA_AMPLITUDE = 1000.0;   // амплитуда сигнала
    const double DOUBLE_MATH_PI = 3.141592653589793238463;

    // подготовим данные для теста
    std::vector<short> test_data_int16; // тестовые данные с разрядностью 16 бит
    std::vector<short> out_data_int16;  // выходные данные с разрядностью 16 бит (буфер сам изменит размер)

    std::vector<double> test_data_double;
    std::vector<double> out_data_double;
    for(int i = 0; i < FREQ_INPUT_DATA; ++i) {
        double temp = INPUT_DATA_AMPLITUDE *
            std::sin((((double)i * INPUT_DATA_FREQ * 2 * DOUBLE_MATH_PI)/(double)FREQ_INPUT_DATA));
        test_data_int16.push_back(temp);
        test_data_double.push_back(temp);
    }

    SrcLinear<short> iSrcLinear(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA);
    SrcLinear<double> iSrcLinearDouble(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA);
    SrcLagrange<short> iSrcLagrange(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 3);
    SrcLagrange<double> iSrcLagrangeDouble(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 3);
    SrcFir<short> iSrcFir(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 1024, 2);
    SrcFir<double> iSrcFirDouble(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 1024, 2);

    if(iSrcFir.init(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 1024, 2) == false) {
        std::cout << "errr" << std::endl;
    }
    std::cout << "Please select a conversion type.\n0  - Linear\n1 - Lagrange\n2 - FIR" << std::endl;
    char s[2];
    while(true) {
        std::cin.getline(s, 2);
        if(s[0] == '0') {
            // запустим преобразование
            cout << iSrcLinear.process(test_data_int16, out_data_int16) << endl;
            cout << iSrcLinearDouble.process(test_data_double, out_data_double) << endl;
            break;
        } else
        if(s[0] == '1') {
            // запустим преобразование
            cout << iSrcLagrange.process(test_data_int16, out_data_int16) << endl;
            cout << iSrcLagrangeDouble.process(test_data_double, out_data_double) << endl;
            break;
        } else
        if(s[0] == '2') {
            // запустим преобразование
            cout << iSrcFir.process(test_data_int16, out_data_int16) << endl;
            cout << iSrcFirDouble.process(test_data_double, out_data_double) << endl;
            break;
        }
        std::cout << "Input Error" << std::endl;
    }

    // посмотрим немного элементов
    size_t pos = 0;
    if(out_data_int16.size() > 0)
    while(true) {
        std::cin.getline(s, 2);
        if(s[0] == 'B' || s[0] == 'b')
            break;
        std::cout << "int: " << out_data_int16[pos] << " float: " << out_data_double[pos] << std::endl;
        pos++;
        if(pos >= out_data_int16.size()) {
            pos = 0;
        }
    }

    return 0;
}
