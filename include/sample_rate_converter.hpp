#ifndef SAMPLE_RATE_CONVERTER_HPP_INCLUDED
#define SAMPLE_RATE_CONVERTER_HPP_INCLUDED

#include <vector>
#include <cmath>
#include <limits>

/** \brief Преобразователь частоты дискретизации
 * с линейной интерполяцией.
 * Выходной сигнал имеет задержку на 1 сэмпл
 */
template <typename T>
class SrcLinear {
private:
    unsigned char m_accuracy;
    long long m_mask;
    long long m_dTf;
    long long m_dT;
    long long m_Tf;
    long long m_T;
    long long m_inT;
    T m_previous;
public:
 //
    SrcLinear() {
        init(1, 1,(unsigned char)16);
    }

    /** \brief Инициализация параметров
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param accuracy - точность
     */
    bool init(int inSampleRate, int outSampleRate, unsigned char accuracy) {
        m_mask = (1 << accuracy) - 1;
        long long quotient = ((long long)inSampleRate << accuracy) / (long long)outSampleRate;
        m_dTf = (long long)(quotient & m_mask);
        m_dT = (long long)(quotient >> accuracy);
        m_accuracy = accuracy;
        m_Tf = 0;
        m_T = 0;
        m_inT = 0;
        m_previous = 0;
        return (quotient > 0);
    }

    /** \brief Инициализация класса
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param accuracy точность
     */
    SrcLinear(int inSampleRate, int outSampleRate, unsigned char accuracy = 16) {
        init(inSampleRate, outSampleRate, accuracy);
    }

    /** \brief Преобразовать частоту дискретизации
     * \param in входной буфер
     * \param out выходной буфер, должен иметь достаточный размер
     * \return количество сэмплов в выходном буфере
     */
    int process(std::vector<T> &in, std::vector<T> &out) {
        int outCount = 0;
        int s = 0;
        while(true) {
            while(m_inT <= m_T) {
                m_previous = in[s++];
                m_inT++;
                if(s >= (int)in.size()) {
                    if(m_inT < m_T) {
                        m_T = m_T - m_inT;
                        m_inT = 0;
                    } else {
                        m_inT = m_inT - m_T;
                        m_T = 0;
                    }
                    return outCount;
                }
            }

            T temp = (T)((long long)m_previous + (long long)(((long long)m_Tf * (long long)((long long)in[s] - (long long)m_previous)) >> m_accuracy));
            out.push_back(temp);
            outCount++;
            //out[outCount++] = (short)((int)m_previous + (int)(((long)m_Tf * (long)((int)in[s] - (int)m_previous)) >> m_accuracy));
            long long t = m_Tf + m_dTf;
            m_T = m_T + m_dT + (long long)(t >> m_accuracy);
            m_Tf = t & m_mask;
        } //while(outCount < out.size());
        /*
        if( m_inT < m_T ) {
            m_T = m_T - m_inT;
            m_inT = 0;
        } else {
            m_inT = m_inT - m_T;
            m_T = 0;
        }

        return outCount;
        */
    }
};

/** \brief Преобразователь частоты дискретизации с интерполяцией по Лагранжу.
 * Выходной сигнал имеет задержку на (N+1)/2 сэмпл.
 */
template <typename T>
class SrcLagrange {
private:
    double m_dT;
    double m_T;
    int m_inT;
    std::vector<double> m_pF;
    unsigned char m_wrPos;
    std::vector<double> m_pLI;
    unsigned char m_interpolation;
    unsigned char m_Hinterpolation;
public:

    SrcLagrange() {
        init(1, 1, (unsigned char)1);
    }

    /** \brief Инициализация параметров
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param interpolation порядок интерполяции
     */
    bool init(int inSampleRate, int outSampleRate, unsigned char interpolation) {
        if((interpolation & 1) == 0) return false;
        m_interpolation = interpolation;
        m_Hinterpolation = (unsigned char)((interpolation - 1) >> 1);
        m_pF.resize(m_interpolation);
        m_pLI.resize(m_interpolation);
        m_wrPos = m_interpolation;
        m_dT = (double)inSampleRate / (double)outSampleRate;
        m_T = 0.0;
        m_inT = 0;
        return (m_dT > 0.0);
    }

    /** \brief Инициализация класса
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param interpolation порядок интерполяции
     */
    SrcLagrange(int inSampleRate, int outSampleRate, unsigned char interpolation = 1) {
        init(inSampleRate, outSampleRate, interpolation);
    }


    /** \brief Преобразовать частоту дискретизации
     * \param in входной буфер
     * \param out выходной буфер, должен иметь достаточный размер
     * \return количество сэмплов в выходном буфере
     */
    int process(std::vector<T> &in, std::vector<T> &out) {
        int outCount = 0;
        int s = 0;
        while(true) {
            while(m_inT <= (int)m_T) {
                m_inT++;
                m_pF[m_wrPos++] = in[s++];
                if(m_wrPos > m_interpolation) m_wrPos = 0;
                if(s >= (int)in.size()) {
                    if( m_inT < m_T ) {
                        m_T = m_T - (double)m_inT;
                        m_inT = 0;
                    } else {
                        m_inT = m_inT - (int)m_T;
                        m_T = m_T - (double)(int)m_T;
                    }
                    return outCount;
                }
            }

            double D = (double)m_Hinterpolation + m_T - (double)(int)m_T;

            for(unsigned char n = 0; n <= m_interpolation; ++n) {
                m_pLI[n] = 1.0;
                for(unsigned char k = 0; k <= m_interpolation; ++k) {
                    if(n != k) {
                        m_pLI[n] = m_pLI[n] * (D - (double)k) / (double)(n - k);
                    }
                }
            }

            double fout = 0.0;

            for(unsigned char k = 0, index = m_wrPos; ; ) {
                fout = fout + m_pLI[k++] * m_pF[index];
                if(k > m_interpolation) break;
                if(++index > m_interpolation) index = 0;
            }

            // ограничение амплитуды
            if(fout > std::numeric_limits<T>::max())
                    fout = std::numeric_limits<T>::max();

            if(fout < std::numeric_limits<T>::lowest())
                    fout = std::numeric_limits<T>::lowest();

            out.push_back(fout);
            outCount++;
            //out[outCount++] = (short)iout;
            //
            m_T = m_T + m_dT;
        } //while(outCount < out.size());
        /*
        if( m_inT < m_T ) {
            m_T = m_T - (double)m_inT;
            m_inT = 0;
        } else {
            m_inT = m_inT - (int)m_T;
            m_T = m_T - (double)(int)m_T;
        }

        return outCount;
        */
    }
};


/** \brief Преобразователь частоты дискретизации с помощью КИХ-фильтра.
 * Выходной сигнал имеет задержку, зависящую от длины фильтра.
 * Для экономии памяти и увеличения качества при небольших значениях
 * oversampling можно добавить вычисление методом линейной интерполяции
 * отсутствующих значений КИХ фильтра.
 */
template <typename T>
class SrcFir {
private:
    unsigned char m_mul_count;
    int m_oversampling;
    int m_fir_length1;
    std::vector<double> m_fir;
    std::vector<double> m_buff;
    double m_dT;
    double m_T;
    int m_inT;

    /** \brief Получить элемент окна Блэкмана
     * \param i номер элемента
     * \param length длина окна
     * \return значение элемента окна Блэкмана
     */
    double get_window_blackman(int i, int length) {
        const double A0 = 0.42;
        const double A1 = 0.5;
        const double A2 = 0.08;
        const double DOUBLE_MATH_PI = 3.141592653589793238463;
        double N_SUB1 = length - 1;
        return A0 - (A1 * std::cos((2.0 * DOUBLE_MATH_PI * (double)i)/N_SUB1)) +
            (A2 * std::cos((4.0 * DOUBLE_MATH_PI * (double)i)/N_SUB1));
    }

public:

    SrcFir() {
        init(1, 1, 1, 1);
    }

    /** \brief Инициализация класса
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param oversampling частота дискретизации фильтра, определяет точность по оси времени.
     * если не используется интерполяция коэффициентов фильтра, стоит использовать значение не менее 1024.
     * в аппаратных ПЧД используется 1048576.
     * таблица коэффициентов фильтра имеет размер oversampling * mul.
     * произведение oversampling * mul должно быть чётным для
     * расчёта фильтра чётного порядка, что позволяет хранить
     * только половину коэффициентов и съэкономить память.
     * \param mul - желаемое число умножений при фильтрации, определяет загрузку процессора.
     */
    SrcFir(int inSampleRate, int outSampleRate, int oversampling, unsigned char mul) {
        init(inSampleRate, outSampleRate, oversampling, mul);
    }


    /** \brief Инициализация параметров
     * \param inSampleRate частота дискретизации входного сигнала
     * \param outSampleRate частота дискретизации выходного сигнала
     * \param oversampling частота дискретизации фильтра, определяет точность по оси времени.
     * если не используется интерполяция коэффициентов фильтра, стоит использовать значение не менее 1024.
     * в аппаратных ПЧД используется 1048576.
     * таблица коэффициентов фильтра имеет размер oversampling * mul.
     * произведение oversampling * mul должно быть чётным для
     * расчёта фильтра чётного порядка, что позволяет хранить
     * только половину коэффициентов и съэкономить память.
     * \param mul - желаемое число умножений при фильтрации, определяет загрузку процессора.
     * \return true, если успешна, иначе false
     */
    bool init(int inSampleRate, int outSampleRate, int oversampling, unsigned char mul) {
        if((mul & oversampling & 1) != 0)
            return false;

        m_oversampling = oversampling;
        m_mul_count = mul;
        m_fir_length1 = m_oversampling * m_mul_count;

        m_fir.resize((m_fir_length1 >> 1));

        // расчитываем фильтр для половины частоты дискретизации
        // так как рабочая частота повышена в oversampling раз,
        // частота фильтра должна быть в oversampling раз меньше.
        // f = 1, fd = 2
        // w = 2 * pi * f / fd / oversampling
        const double DOUBLE_MATH_PI = 3.141592653589793238463;
        double w = DOUBLE_MATH_PI / m_oversampling;

        double sum = 0.0;
        // для симметричной характеристики
        double c = (double)m_fir.size() - 0.5;
        for(size_t i = 0; i < m_fir.size(); ++i) {
            double d = (double)i - c;
            m_fir[i] = std::sin(w * d) / d * get_window_blackman(i, m_fir_length1);
            sum += m_fir[i] + m_fir[i];
        }

        m_fir_length1--;

        // нормализация и масштабирование
        sum /= oversampling;
        for(size_t i = 0; i < m_fir.size(); ++i)
            m_fir[i] /= sum;

        m_buff.resize(m_mul_count);

        for(int i = m_mul_count; i-- > 0;) m_buff[i] = 0;
        m_dT = (double)inSampleRate / (double)outSampleRate;
        m_T = 0.0;
        m_inT = 0;
        return true;
    }

    /** \brief Преобразовать частоту дискретизации
     * \param in входной буфер
     * \param out выходной буфер, должен иметь достаточный размер
     * \return количество сэмплов в выходном буфере
     */
    int process(std::vector<T> &in, std::vector<T> &out) {
         int outCount = 0;
         int s = 0;
         while(true) {
            while(m_inT <= m_T) {
                m_inT++;

                // буфер для фильтрации, линия задержки
                for(int i = m_mul_count; --i > 0;)
                    m_buff[i] = m_buff[i - 1];

                m_buff[0] = in[s++];
                if(s >= (int)in.size()) {
                    if( m_inT < m_T ) {
                        m_T = m_T - (double)m_inT;
                        m_inT = 0;
                    } else {
                        m_inT = m_inT - (int)m_T;
                        m_T = m_T - (double)(int)m_T;
                    }
                    return outCount;
                }
            }

            // ближайшее начальное смещение по временной оси
            int shift = (int)(0.5 + m_oversampling * (m_T - (double)(int)m_T));
            // для увеличения точности при небольшом значении oversampling
            // можно дополнительно расчитать методом интерполяции
            // значение коэффицента фильтра в требуемой точке.
            // фильтрация
            double fout = 0.0;
            int k, i;
            // первая половина КИХ-фильтра
            for(k = shift, i = 0; k < (int)m_fir.size(); k += m_oversampling)
                fout += m_fir[k] * m_buff[i++];
            // вторая половина КИХ-фильтра
            for( k = m_fir_length1 - k; k >= 0; k -= m_oversampling)
                fout += m_fir[k] * m_buff[i++];

            // ограничение амплитуды
            if(fout > std::numeric_limits<T>::max())
                    fout = std::numeric_limits<T>::max();

            if(fout < std::numeric_limits<T>::lowest())
                    fout = std::numeric_limits<T>::lowest();

            out.push_back(fout);
            outCount++;
            //out[outCount++] = (short)fout;
            //
            m_T = m_T + m_dT;
        }
         //while( outCount < out.length );
        /*
        if( m_inT < m_T ) {
            m_T = m_T - (double)m_inT;
            m_inT = 0;
        } else {
            m_inT = m_inT - (int)m_T;
            m_T = m_T - (double)(int)m_T;
        }
        return outCount;
        */
    }
};

#endif // SAMPLE_RATE_CONVERTER_HPP_INCLUDED
