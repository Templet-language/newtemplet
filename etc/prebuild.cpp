// предполагается, что в каталоге utils находятся утилиты из пакета newtemplet:
// skel -- собирается из newtemplet/dsl/skel.cpp
// gen  -- собирается из newtemplet/dsl/gen.cpp, newtemplet/dsl/lexer.cpp, newtemplet/dsl/parse.cpp
// dtaskbag.dsg.cpp -- это newtemplet\samples\dtaskbag\dtaskbag.dsg.cpp
// taskbag.dsg.cpp  -- это newtemplet\samples\taskbag\taskbag.dsg.cpp
// pipeline.dsg.cpp -- это newtemplet\samples\pipeline\pipeline.dsg.cpp
// dtaskbag.dpl.cpp -- это newtemplet\samples\dtaskbag\dtaskbag.dpl.cpp
// taskbag.dpl.cpp  -- это newtemplet\samples\taskbag\taskbag.dpl.cpp
// pipeline.dpl.cpp -- это newtemplet\samples\pipeline\pipeline.dpl.cpp


// функция prebuild_design должна быть реализована в виде скрипта,
// хранящегося в отдельном файле prebuild_design.sh

void prebuild_design()
{
	for (auto file : "dir *.cpp") {// для всех файлов с расширением .cpp

		auto first_block_index = exec("utils/skel - i " + file); 
		    // запускаем утилиту skel, она выведет на консоль индекс первого блока
		    // этот индекс позволяет опередилить, какой шаблон используется
		    // если file -- обычный файл, skel выведет в консоль NOMARKUP

		switch (first_block_index) {
		
		case "actor" :
			
			cout << exec("utils/gen -design "+file+" "+file".dsg");
			// по дерективам '#paragma templet' в файле file генерируется шаблон, сохраняемый в файл file.dsg

			cout << exec("utils/skel -i "+file+" -s "+file".dsg");
			// шаблон из file.dsg применяется к исходному файлу file (файл при этом изменяется)

			exec("del file" + ".dsg");
			// файл шаблона file.dsg удаляется

			break;
		
		case "dtaskbag" :

			cout << exec("utils/skel -i " + file + " -s utils/dtaskbag.dsg.cpp");
			// применяем к файлу file шаблон 'распределённый портфель задач' из файла dtaskbag.dsg.cpp

			break;
		
		case "pipeline" :
			
			cout << exec("utils/skel -i " + file + " -s utils/pipeline.dsg.cpp");
			// применяем к файлу file шаблон 'конвейер' из файла pipeline.dsg.cpp

			break;

		case "taskbag" :

			cout << exec("utils/skel -i " + file + " -s utils/taskbag.dsg.cpp");
			// применяем к файлу file шаблон 'портфель задач для разделяемой памяти' из файла taskbag.dsg.cpp
			break;
		
		case "NOMARKUP" :
			// пропускаем файл file
		}
	}
}

// функция аналогична prebuild_design, только используются шаблоны с расширением .dpl.cpp и ключ -deploy в утилите gen
void prebuild_deploy()
{
	for (auto file : "dir *.cpp") {

		auto first_block_index = exec("utils/skel - i " + file);

		switch (first_block_index) {

		case "actor":

			cout << exec("utils/gen -deploy " + file + " " + file".dpl");
			cout << exec("utils/skel -i " + file + " -s " + file".dpl");
			exec("del file" + ".dpl");

			break;

		case "dtaskbag":

			cout << exec("utils/skel -i " + file + " -s utils/dtaskbag.dpl.cpp");
			break;

		case "pipeline":

			cout << exec("utils/skel -i " + file + " -s utils/pipeline.dpl.cpp");
			break;

		case "taskbag":

			cout << exec("utils/skel -i " + file + " -s utils/taskbag.dpl.cpp");
			break;

		case "NOMARKUP":
			// пропускаем файл file
		}
	}
}