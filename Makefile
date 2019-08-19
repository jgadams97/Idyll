make:
	gcc -Wall src/main.c -o build/idyll -DDESKTOP

dynamic:
	gcc -Wall src/main_dynamic.c -o build/idyll -DDESKTOP
