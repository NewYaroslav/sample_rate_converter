# sample_rate_converter
Изменение частоты дискретизации

### Описание

Данная *header-only* C++ библиотека содержит классы для изменение частоты дискретизации несколькими методами. 
На данный момент поддерживаются следующие варианты:

* Реализация для линейной интерполяции (класс *SRCLinear*)
* Реализация для интерполяции по Лагранжу (класс *SRCLagrange*)
* Реализация для интерполяции c помощью КИХ-фильтра (класс *SRCFIR*)

### Как пользоваться

Примерно вот так:

```C++
#include "sample_rate_converter.hpp" // подключаем эту библиотеку

// дальше кодим
std::vector<double> test_data_double;
std::vector<double> out_data_double;
//...

SrcLagrange<double> iSrcLagrangeDouble(FREQ_INPUT_DATA, FREQ_OUTPUT_DATA, 3);
iSrcLagrangeDouble.process(test_data_double, out_data_double);

```

### ПО

В каталоге *code_blocks\sr_converter* расположен исходный код консольной программы для изменения частоты дискретизации сигналов, представленных в виде массива чисел, представенных в текстовом виде.

Пример использования:

```
-in input_file.txt -out output_file.txt -fi 4000 -fo 1200 -fir -d -r 1024 -m 1
```

* *input_file.txt* - исходный файл с сэмплами сигнала
* *output_file.txt* - файл с новой частотой дискретизации сигнала 
* *-fi 4000* - частота сигнала на входе
* *-fo 1200* - частота сигнала на выходе
* *-fir* - метод преобразования 
* *-d* - использовать тип double для сигнала на входе и выходе
* *-r 1024* - частота дискретизации фильтра, определяет точность по оси времени
* *-m 1* - желаемое число умножений при фильтрации, определяет загрузку процессора

### Полезные ссылки

* Первоисточник кода: [http://dmilvdv.narod.ru/SpeechSynthesis/resampling.html](http://dmilvdv.narod.ru/SpeechSynthesis/resampling.html)
* Secret Rabbit Code: [http://www.mega-nerd.com/SRC/](http://www.mega-nerd.com/SRC/)
* Некоторые оконные функции и их параметры: [http://www.dsplib.ru/content/winadd/win.html](http://www.dsplib.ru/content/winadd/win.html)
