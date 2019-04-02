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

```


### Полезные ссылки

* Первоисточник кода: [http://dmilvdv.narod.ru/SpeechSynthesis/resampling.html](http://dmilvdv.narod.ru/SpeechSynthesis/resampling.html)
* Secret Rabbit Code: [http://www.mega-nerd.com/SRC/](http://www.mega-nerd.com/SRC/)
