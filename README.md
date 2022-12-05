<b><u>LLP 1:</u></b>
<hr>
<b>a. Параметры для всех операций задаются посредством формирования соответствующих структур
данных.</b>
<br>
<br>
Для операций создания и вставки используется структура Table, описанная в db_internals.h.<br>
Для выборки, обновления и удаления используются фильры из db_interface.
Ниже указаны сами функции.
<br>

<i>Adds a new table header to the file and creates a link for it in the root page. Exits if the table already exists.
(CREATE TABLE ... VALUES(...)).</i><br>
<u>void addTableHeader(const char *filename, Table *table);</u>

<i>Appends all the records into the table. Does nothing if the table does not exist.
(INSERT INTO ... VALUES(...)).</i><br>
<u>void insertTableRecords(const char *filename, Table *table);</u>

<i>Outputs all the records of the table filtering them through the provided filter.
(SELECT * FROM ... WHERE ...).</i><br>
<u>void printTable(const char *filename, const char *table_name, size_t num_of_filters, SearchFilter **filters);</u>

<i>Updates filtered rows.
(UPDATE ... FROM ... WHERE ...).</i><br>
<u>void updateRows(const char *filename, const char *table_name, TableRecord *new_value, size_t num_of_filters, SearchFilter **filters);</u>

<i>Deletes filtered rows. If the removal empties a page, it gets expelled from the thread and can be reused later.
(DELETE FROM ... WHERE ...).</i><br>
<u>void deleteRows(const char *filename, const char *table_name, size_t num_of_filters, SearchFilter **filters);</u>
<hr>

<b>b. Показать, что при выполнении операций, результат выполнения которых не отражает
отношения между элементами данных, потребление оперативной памяти стремится к O(1)
независимо от общего объёма фактического затрагиваемых данных.</b>
<hr>

<b>c. Показать, что операция вставки выполняется за O(1) независимо от размера данных,
представленных в файле.</b>
<br>
График времени выполнения от номера таблицы, добавление происходит к базе с начальным размером таблицы в 25000 элементов. 
Запуски происходят с ключом -i, который вызывает вставку порции элементов в каждую таблицу.
На графике видно, что время вставки не зависит от размера данных, которые лежат перед этой таблицей, 
а колебания появляются из-за разного количества полей.
![](https://github.com/Yars2021/llp_lab1/blob/main/images/insertion_line.png)
<hr>

<b>d. Показать, что операция выборки без учёта отношений (но с опциональными условиями)
выполняется за O(n), где n – количество представленных элементов данных выбираемого вида.</b>
<hr>

<b>e. Показать, что операции обновления и удаления элемента данных выполняются не более чем за
O(n*m) > t -> O(n+m), где n – количество представленных элементов данных обрабатываемого
вида, m – количество фактически затронутых элементов данных.</b>
<hr>

<b>f. Показать, что размер файла данных всегда пропорционален количеству фактически
размещённых элементов данных.</b>
<br>
<br>
Снизу представлен график зависимости размера файла от размера данных. Зависимость стремится к линейной.
Небольшая кривизна в начале возникает из-за страниц с метаинформацией, которые занимают относительно много места, если размеры тестов маленькие.
<br>
![](https://github.com/Yars2021/llp_lab1/blob/main/images/mem_usage_line.png)

<hr>

<b>g. Показать работоспособность решения под управлением ОС семейств Windows и *NIX</b><br>
<br>
Вышеуказанные результаты тестов говорят о работоспособности решения.