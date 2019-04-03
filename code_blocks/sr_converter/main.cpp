#include <iostream>
#include "sample_rate_converter.hpp"
#include <fstream>

enum VariableType {
    SET_8BIT = 0,
    SET_16BIT,
    SET_32BIT,
    SET_FLOAT,
    SET_DOUBLE
};

enum SrcType {
    SET_LINEAR = 0,
    SET_LAGRANGE,
    SET_FIR,
};

// для проверки аргумента
bool check_arg_size(int indx, int size);
bool check_freq(int freq);
bool check_type(int type);
bool check_accuracy(int accuracy);
bool check_interpolation(int interpolation);
bool check_oversampling(int oversampling);
bool check_mul(int mul);

// функция для открытия файла
template <typename T>
bool open_file(std::string file_name, std::vector<T> &data) {
    std::ifstream file;
    file.open(file_name);
    if(!file.is_open()) {
        std::cout << "error: can not open file: " << file_name << std::endl;
        return false;
    }
    while(!file.eof()) {
        std::string word;
        file >> word;
        if(std::is_same<T, float>::value || std::is_same<T, double>::value) {
            T temp = std::atof(word.c_str());
            data.push_back(temp);
        } else {
            T temp = std::atoi(word.c_str());
            data.push_back(temp);
        }
    }
    file.close();
    return true;
}

// функция для сохранения файла
template <typename T>
bool save_file(std::string file_name, std::vector<T> &data) {
    std::ofstream file(file_name);
    if(!file.is_open()) {
        std::cout << "error: can not open file: " << file_name << std::endl;
        return false;
    }
    if(data.size() == 0) {
        std::cout << "error: no data" << std::endl;
        return false;
    }
    for(size_t n = 0; n < data.size(); ++n) {
        file << std::to_string(data[n]) << std::endl;
    }
    file.close();
    return true;
}

// функция для обработки файлов методом fir
template <typename T1>
bool calc_src_fir(
        std::string input_file_name,
        std::string output_file_name,
        int freq_in,
        int freq_out,
        int oversampling,
        unsigned char mul) {
    SrcFir<T1> iSrc(freq_in, freq_out, oversampling, mul);
    std::vector<T1> in, out;
    if(!open_file(input_file_name, in)) return false;
    int res = iSrc.process(in, out);
    if(!save_file(output_file_name, out)) return false;
    std::cout << "data saved to file: " << output_file_name << std::endl;
    std::cout << "input data length: " << in.size() << std::endl;
    std::cout << "output data length: " << res << std::endl;
    return true;
}

// функция для обработки файлов методом Linear и Lagrange
template <typename T1, template<typename> class T2>
bool calc_src_linear_and_lagrange(
        std::string input_file_name,
        std::string output_file_name,
        int freq_in,
        int freq_out,
        int param) {
    T2<T1> iSrc(freq_in, freq_out, param);
    std::vector<T1> in, out;
    if(!open_file(input_file_name, in)) return false;
    int res = iSrc.process(in, out);
    if(!save_file(output_file_name, out)) return false;
    std::cout << "data saved to file: " << output_file_name << std::endl;
    std::cout << "input data length: " << in.size() << std::endl;
    std::cout << "output data length: " << res << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if(argc == 0)
        return 0;

    std::string input_file_name, output_file_name;
    int freq_in = 0, freq_out = 0, type = SET_LINEAR, accuracy = 16;
    int variable_type = SET_16BIT;  // тип переменной
    int interpolation = 3;          // интерполяция для метода Lagrange
    int oversampling = 1024;        // частота дискретизации фильтра, определяет точность по оси времени.
    int mul = 1;                    // желаемое число умножений при фильтрации, определяет загрузку процессора

    for(int i = 0; i < argc; i++) {
        std::string input_arg(argv[i]);
        if(input_arg == "help") {
            std::cout << "example: " << std::endl;
            std::cout << std::endl;
            std::cout << "-in input_file.txt -out output_file.txt -fi 4000 -fo 1200" << std::endl;
            std::cout << "-in input_file.txt -out output_file.txt -fi 4000 -fo 1200 -t 0 -i16" << std::endl;
            std::cout << "-in input_file.txt -out output_file.txt -fi 4000 -fo 1200 -t 0 -f" << std::endl;
            std::cout << "-in input_file.txt -out output_file.txt -fi 4000 -fo 1200 -t 2 -d -r 1024 -m 1" << std::endl;
            std::cout << "-in input_file.txt -out output_file.txt -fi 4000 -fo 1200 -fir -d -r 1024 -m 1" << std::endl;
            std::cout << std::endl;
            std::cout << "defaults: -t 0 -i16 -a 16 -i 3 -r 1024 -" << std::endl;
            std::cout << std::endl;
            std::cout << "command list: " << std::endl;
            std::cout << std::endl;
            std::cout << "-in <file name>        - file for conversions" << std::endl;
            std::cout << "-out <file name>       - file to save the result" << std::endl;
            std::cout << std::endl;
            std::cout << "-fi <frequency>       - input sample rate" << std::endl;
            std::cout << "-fo <frequency>       - output sample rate" << std::endl;
            std::cout << std::endl;
            std::cout << "-t <number>           - conversion type (0 = Linear; 1 = Lagrange; 2 = FIR)" << std::endl;
            std::cout << std::endl;
            std::cout << "-linear               - conversion type 'Linear'" << std::endl;
            std::cout << "-accuracy <number>    - accuracy for conversion type 'Linear'" << std::endl;
            std::cout << "-a <number>           - accuracy for conversion type 'Linear'" << std::endl;
            std::cout << std::endl;
            std::cout << "-lagrange             - conversion type 'Lagrange'" << std::endl;
            std::cout << "-interp <number>      - interpolation order for conversion type 'Lagrange'" << std::endl;
            std::cout << "-i <number>           - interpolation order for conversion type 'Lagrange'" << std::endl;
            std::cout << std::endl;
            std::cout << "-fir                  - conversion type 'FIR'" << std::endl;
            std::cout << "-r <number>           - FIR sampling rate,\n determines the accuracy along the time axis" << std::endl;
            std::cout << "-m <number>           - the desired number of multiplications when filtering\n determines the CPU load (for FIR)" << std::endl;
            std::cout << std::endl;
            std::cout << "-int32                - use integer type (int32)" << std::endl;
            std::cout << "-i32                  - use integer type (int32)" << std::endl;
            std::cout << "-int16                - use integer type (int16)" << std::endl;
            std::cout << "-i16                  - use integer type (int16)" << std::endl;
            std::cout << "-int8                 - use integer type (int8)" << std::endl;
            std::cout << "-i8                   - use integer type (int8)" << std::endl;
            std::cout << "-float                - use floating point type (float)" << std::endl;
            std::cout << "-f                    - use floating point type (float)" << std::endl;
            std::cout << "-double               - use floating point type (double)" << std::endl;
            std::cout << "-d                    - use floating point type (double)" << std::endl;
            std::cout << std::endl;
            if(argc == 1)
                return 0;
        } else

        if(input_arg == "-in") {
            if(!check_arg_size(i, argc)) return 0;
            input_file_name = std::string(argv[i + 1]);
        } else
        if(input_arg == "-out") {
            if(!check_arg_size(i, argc)) return 0;
            output_file_name = std::string(argv[i + 1]);
        } else

        if(input_arg == "-fi") {
            if(!check_arg_size(i, argc)) return 0;
            freq_in = atoi(argv[i + 1]);
            if(!check_freq(freq_in)) return 0;
        } else
        if(input_arg == "-fo") {
            if(!check_arg_size(i, argc)) return 0;
            freq_out = atoi(argv[i + 1]);
            if(!check_freq(freq_in)) return 0;
        } else

        if(input_arg == "-t") {
            if(!check_arg_size(i, argc)) return 0;
            type = atoi(argv[i + 1]);
            if(!check_type(type)) return 0;
        } else
        if(input_arg == "-linear") {
            type = SET_LINEAR;
        } else
        if(input_arg == "-lagrange") {
            type = SET_LAGRANGE;
        }
        if(input_arg == "-fir") {
            type = SET_FIR;
        } else

        if(input_arg == "-accuracy" || input_arg == "-a") {
            if(!check_arg_size(i, argc)) return 0;
            accuracy = atoi(argv[i + 1]);
            if(!check_accuracy(accuracy)) return 0;
        } else
        if(input_arg == "-interp" || input_arg == "-interpolation" || input_arg == "-i") {
            if(!check_arg_size(i, argc)) return 0;
            interpolation = atoi(argv[i + 1]);
            if(!check_interpolation(interpolation)) return 0;
        } else
        if(input_arg == "-r" || input_arg == "-oversampling") {
            if(!check_arg_size(i, argc)) return 0;
            oversampling = atoi(argv[i + 1]);
            if(!check_oversampling(oversampling)) return 0;
        } else
        if(input_arg == "-mul" || input_arg == "-mul") {
            if(!check_arg_size(i, argc)) return 0;
            mul = atoi(argv[i + 1]);
            if(!check_mul(mul)) return 0;
        } else

        if(input_arg == "-int32" || input_arg == "-i32") {
            variable_type = SET_32BIT;
        } else
        if(input_arg == "-int16" || input_arg == "-i16") {
            variable_type = SET_16BIT;
        } else
        if(input_arg == "-int8" || input_arg == "-i8") {
            variable_type = SET_8BIT;
        } else
        if(input_arg == "-float" || input_arg == "-f") {
            variable_type = SET_FLOAT;
        } else
        if(input_arg == "-double" || input_arg == "-d") {
            variable_type = SET_DOUBLE;
        }
    }
    // сначала проверим необходимый набор параметров
    if(freq_in == 0 || freq_out == 0) {
        std::cout << "error: no sampling rate specified in parameters" << std::endl;
        return 0;
    }
    if(input_file_name.size() == 0 || output_file_name.size() == 0) {
        std::cout << "error: no files specified in parameters" << std::endl;
        return 0;
    }
    std::cout << "you selected:" << std::endl;
    std::cout << "input sampling rate: " << freq_in << std::endl;
    std::cout << "output sampling rate: " << freq_out << std::endl;
    std::cout << "type: ";
    if(type == SET_LINEAR) {
        std::cout << "linear" << std::endl << "accuracy: " << accuracy << std::endl;
    } else
    if(type == SET_LAGRANGE) {
        std::cout << "lagrange" << std::endl << "interpolation: " << interpolation << std::endl;
    } else
    if(type == SET_FIR) {
        std::cout << "fir" << std::endl << "oversampling: " << oversampling << std::endl << "mul: " << mul << std::endl;
    }

    if(variable_type == SET_8BIT) {
        std::cout << "data type: int8";
    } else
    if(variable_type == SET_16BIT) {
        std::cout << "data type: int16";
    } else
    if(variable_type == SET_32BIT) {
        std::cout << "data type: int32";
    } else
    if(variable_type == SET_FLOAT) {
        std::cout << "data type: float";
    } else
    if(variable_type == SET_DOUBLE) {
        std::cout << "data type: double";
    }
    std::cout << std::endl;

    bool state = true;
    // начнем преобразование
    if(type == SET_LINEAR) {

        if(variable_type == SET_8BIT) {
            state = calc_src_linear_and_lagrange<char, SrcLinear>(input_file_name, output_file_name, freq_in, freq_out, accuracy);
        } else
        if(variable_type == SET_16BIT) {
            state = calc_src_linear_and_lagrange<short, SrcLinear>(input_file_name, output_file_name, freq_in, freq_out, accuracy);
        } else
        if(variable_type == SET_32BIT) {
            state = calc_src_linear_and_lagrange<long, SrcLinear>(input_file_name, output_file_name, freq_in, freq_out, accuracy);
        } else
        if(variable_type == SET_FLOAT) {
            state = calc_src_linear_and_lagrange<float, SrcLinear>(input_file_name, output_file_name, freq_in, freq_out, accuracy);
        } else
        if(variable_type == SET_DOUBLE) {
            state = calc_src_linear_and_lagrange<double, SrcLinear>(input_file_name, output_file_name, freq_in, freq_out, accuracy);
        }
    } else
    if(type == SET_LAGRANGE) {
        if(variable_type == SET_8BIT) {
            state = calc_src_linear_and_lagrange<char, SrcLagrange>(input_file_name, output_file_name, freq_in, freq_out, interpolation);
        } else
        if(variable_type == SET_16BIT) {
            state = calc_src_linear_and_lagrange<short, SrcLagrange>(input_file_name, output_file_name, freq_in, freq_out, interpolation);
        } else
        if(variable_type == SET_32BIT) {
            state = calc_src_linear_and_lagrange<long, SrcLagrange>(input_file_name, output_file_name, freq_in, freq_out, interpolation);
        } else
        if(variable_type == SET_FLOAT) {
            state = calc_src_linear_and_lagrange<float, SrcLagrange>(input_file_name, output_file_name, freq_in, freq_out, interpolation);
        } else
        if(variable_type == SET_DOUBLE) {
            state = calc_src_linear_and_lagrange<double, SrcLagrange>(input_file_name, output_file_name, freq_in, freq_out, interpolation);
        }
    }
    if(type == SET_FIR) {
        if(variable_type == SET_8BIT) {
            state = calc_src_fir<char>(input_file_name, output_file_name, freq_in, freq_out, oversampling, mul);
        } else
        if(variable_type == SET_16BIT) {
            state = calc_src_fir<short>(input_file_name, output_file_name, freq_in, freq_out, oversampling, mul);
        } else
        if(variable_type == SET_32BIT) {
            state = calc_src_fir<long>(input_file_name, output_file_name, freq_in, freq_out, oversampling, mul);
        } else
        if(variable_type == SET_FLOAT) {
            state = calc_src_fir<float>(input_file_name, output_file_name, freq_in, freq_out, oversampling, mul);
        } else
        if(variable_type == SET_DOUBLE) {
            state = calc_src_fir<double>(input_file_name, output_file_name, freq_in, freq_out, oversampling, mul);
        }
    }
    if(!state)
        return 0;
    std::cout << "conversion completed!" << std::endl;
    return 0;
}

bool check_arg_size(int indx, int size) {
    if((indx + 1) >= size) {
        std::cout << "error: incorrect parameters" << std::endl;
        return false;
    }
    return true;
}

bool check_freq(int freq) {
    if(freq < 0 || freq == 0) {
        std::cout << "error: incorrect frequency" << std::endl;
        return false;
    }
    return true;
}

bool check_type(int type) {
    if(type < 0 || type > 2) {
        std::cout << "error: incorrect type" << std::endl;
        return false;
    }
    return true;
}

bool check_accuracy(int accuracy) {
    if(accuracy <= 1 || accuracy > 32) {
        std::cout << "error: incorrect accuracy" << std::endl;
        return false;
    }
    return true;
}

bool check_interpolation(int interpolation) {
    if(interpolation <= 1 || interpolation % 2 == 0) {
        std::cout << "error: incorrect interpolation" << std::endl;
        return false;
    }
    return true;
}

bool check_oversampling(int oversampling) {
    if(oversampling <= 1) {
        std::cout << "error: incorrect oversampling" << std::endl;
        return false;
    }
    return true;
}

bool check_mul(int mul) {
    if(mul < 0) {
        std::cout << "error: incorrect mul" << std::endl;
        return false;
    }
    return true;
}


