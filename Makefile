#Copyright Ionut Mantu 313 CAb 2023-2024
build:
	gcc image_editor.c -g -Wall -Wextra -lm -o image_editor
clean:
	rm image_editor