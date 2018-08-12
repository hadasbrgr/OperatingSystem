ex51: (keyboard side)
The program produces a process in which it executes the out.draw file (exec file ex25)
The program will listen to every character that the user types.
When the user types one of the following characters: q, w, d, s, a The program writes to the pipe the received character and sends a sigusr2 signal to the son process.
When the user types the q character, the program finishes its operation.

ex52: (Display Side)
The code writes a program which displays a Tetris tablet screen and updates the clipboard accordingly.
The program drops the shape row every second.
When the program receives a sigusr2 signal, it reads from the pipe what is sent and operates accordingly using the io redirection.
For the character a: the program moves the shape from a slot to the left
For the character d: the program moves a slot to the right
For the character s: the program moves the shape from the slot below
For receiving the character w: the program becomes the shape
For q: the program finishes
