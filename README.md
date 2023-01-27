# My Assembler Compiler
An assembler compiler with special instruction sets written in C language.

## Tokenizer
Let's run the makefile first and then the main.exe.
```
mingw32-make & main
```
&nbsp;

Let's write the path of the file to be tokenized in the path part. (if the file is in the same directory you can just type its name)
```
source
```
### SS
![image](https://user-images.githubusercontent.com/71611710/215190992-a55f4913-0435-4c2e-b62b-64d1ec5a3b55.png)
&nbsp;

## Parser
Compile and run.
```
gcc parser.c -o parser & parser
```
&nbsp;

Let's write the path of the file to be compiled in the path part. (if the file is in the same directory, just type its name)
```
source2
```
### SS
![image](https://user-images.githubusercontent.com/71611710/215195209-399bd11f-990d-43dd-b236-43a01f588435.png)

&nbsp;

## Assembler
Compile and run.
```
gcc assembler.c -o assembler & assembler
```
&nbsp;

Let's write the path of the file to be compiled in the path part. (if the file is in the same directory, just type its name)
```
source3
```
&nbsp;

source3 - Program to add numbers from 1 to 10 - The result is inside the AX register
```
HRK AX, 0
HRK BX, 1
HRK CX, 11
loop:
	TOP AX, BX
	TOP BX, 1
	HRK DX, BX
	CIK DX, CX
	SN loop
```

### SS
![image](https://user-images.githubusercontent.com/71611710/215197509-30974e41-2242-403d-abf6-6bc8962fc97d.png)
&nbsp;

## Task
> Add input and output buffers. Nice toy for those who are interested :)

