## command completion code
Напишите программу, которая запускает команду, заданную в качестве первого аргумента, в виде порожденного процесса. Все остальные аргументы программы передаются этой команде. Затем программа должна дождаться завершения порожденного процесса и распечатать его код завершения.
## creating two processes
Напишите программу, которая создает подпроцесс. Этот подпроцесс должен исполнить cat(1) длинного файла. Родитель должен вызвать printf(3) и распечатать какой-либо текст. После выполнения первой части задания модифицируйте программу так, чтобы последняя строка, распечатанная родителем, выводилась после завершения порожденного процесса. Используйте wait(2), waitid(2) или waitpid(3).
## function execvpe()
Напишите функцию execvpe(), которая работает как execvp(2), но позволяет изменять среду исполнения, как execve(2).
## search table for strings in a text file.
Написать программу, которая анализирует текстовый файл, созданный текстовым редактором, таким как ed(1) или vi(1). После запроса, который предлагает ввести номер строки, с использованием printf(3) программа печатает соответствующую строку текста. Ввод нулевого номера завершает работу программы. Используйте open(2), read(2), lseek(2) и close(2) для ввода/вывода. Постройте таблицу отступов в файле и длин строк для каждой строки файла. Как только эта таблица построена, позиционируйтесь на начало заданной строки и прочтите точную длину строки.

Измените программу так, чтобы пользователю отводилось 5 секунд на ввод номера строки. Если пользователь не успевает, программа должна распечатать все содержимое файла и завершиться. Если же пользователь успел в течение пяти секунд ввести номер строки, то программа должна работать как в предыдущей задаче.
