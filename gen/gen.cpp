#pragma templet ~message
#pragma templet ~message =
#pragma templet ~message@(submessage1?,submessage2!,submessage3!,-submessage4?)

#pragma templet *actor
#pragma templet *actor +
#pragma templet *actor@(?,port?message,port!message) +

#pragma templet '~' id ['@'] [ '(' ['-'] id ('!'|'?') {',' ['-'] id ('!'|'?')} ')' ] ['=']
#pragma templet '*' id ['@'] [ '(' ('?'| id ('!'|'?') id) {',' id ('!'|'?') id)} ')' ] ['+']

1) bool openparse(string&name, string&pragma);
// открыть файл для разбора по имени,
// pragma – обрабатывать только заданные прагмы, например, #pragma omp

2) bool getpragma(string&argstring,int&line);
// argstring -- аргументы следующей по порядку разбора команды #pragma,
// line – номер строки файла, с которой начинается прагма

3) void closeparse();
// закрытие файл

1)    void lexinit(string&s);
// инициализация; s – строка символов для разбора
2)    bool getlex(string&lex);
// извлечение следующей лексемы из строки
// return false – если лексем больше нет
3)    bool ungetlex();
// возврат на одну лексему назад
// return false – если возврат невозможен (например, уже выполнен)
4)    void lexclear();
// очистка

