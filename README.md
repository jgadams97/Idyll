IdyllicBASIC is a variant of BASIC that encapsulates memory management.

This means two things: 1. Idyllic scripts can be executed from program memory without loading them into RAM. 2. Idyllic scripts can store its variables within external memory devices. This includes anything from an external RAM to even a hard disk drive or an SD card.

Here is IdyllicBASIC being ran on an ATMega328P microcontroller using a 23LC1024 for external RAM.

![alt text](https://i.imgur.com/5ZKFANj.png)

Both of these things make Idyllic ideal for low-memory applications. The ATMega328P microcontroller, for example, has 2 KB of RAM. Normally, this would be not be enough to run BASIC scripts, as you could only fit a few lines of code into RAM.

However, Idyllic can make use of external RAMs and program memory. The BASIC program does not need to be loaded into RAM to run, and all variables could be stored in an external RAM. The 23LC1024, for example, can store 1 megabit of RAM, which can be used for variable storage with Idyllic. Idyllic stores variables using 32-bit pointers, so it can handle external memory devices up to 4.2 gigabytes. It can easily be recompiled with 64-bit pointers if need be. Idyllic also stores these variables using AVL trees, so lookup times are fast.

Variables in Idyllic can be declared with the DIM command.
```
dim myVar
dim $myVar
```
There are three main data types in Idyllic: 1. Numbers. 2. Strings. 3. Addresses.

Only numbers and strings can be directly manipulated by the user. Strings begin with a "$" character. Addresses begin "@". Numbers begin with nothing.

Values can be assigned to variables with the equal sign.
```
myVar = 5.2
$myVar = "Hello, World!"
```
You can also assign values on the same line you declare the variable.
```
dim myVar = 5.2
dim $myVar = "Hello, World!"
```
Strings can be provided a size when declaring them. In the case above, the string is not provided a size, so it defaults to the size of the "Hello, World!" string. 

Trying to store a string into a string variable larger than it will cause the string to expand in size.

```
dim $myVar = "Hello, "
$myVar = $myVar + "World!"
```

Strings can have a specific size specified in their declaration.

```
dim $myVar[10]
```

You cannot "dim" an address. Addresses are created like so:

```
@myAddress
```
The address automatically gets assigned its value, which depends on where it occurs in the file.

Addresses can be jumped to using "goto @myAddress". This can be used for looping.
```
dim x = 0
@loop
	print x
	x = x + 1
if x < 10 then
	goto @loop
fi
```
This will print numbers 0 through 9.

The IF command can be used for conditional statements. The block of code between IF and FI will get executed only if the condition between IF and "then" is true.

The format of an IF command is:
```
if LHS comparator RHS then
	...
fi
```
Valid comparators are:
```
>=	Greater than or equal to.
<=	Less than or equal to.
>	Greater than.
<	Less than.
!=	Not equal to.
```
Only "==" and "!=" are valid for string comparisons. String comparisons only apply to the first 8 characters of the string.

Some commands output values. These values can be redirected into a variable using the "=>" symbol.
```
dim $name[20]
$read => $name
print "Hello, " + $name + "!"
```
Scripts in their own file can be ran using the "run" command.
```
run "fibonacci.ib"
```

If a number variable is declared with a size, it will become an array.
```
dim x[10]
x[5] = 12
x[2] = 8
print x[5] + x[2]
```
Arrays cannot have a size of 0. 

Array indices can either be numbers or a variable.

```
;Declare the array.
dim myArray[10]

;Fill the array.
dim i = 1
@loop1
	myArray[i] = i * 2
	i = i + 1
if i <= 10 then
	goto @loop1
fi

;Print out the numbers.
i = 1
@loop2
	print myArray[i]
	i = i + 1
if i <= 10 then
	goto @loop2
fi
```
Arrays are 1-indexed. You can also see here that comments begin with a semicolon.

Assigning a number array a value is valid. The number will simply be placed at the first index of the array.

```
dim x[10]
x = 12
print x[0]
print x
```

Strings can also be referenced using an index. This will return that single character at the given index.

```
dim $str = "Hello!"
print $str[1]
```

You may also assign that character a new value. 

```
dim $str = "Hello!"
$str[2] = "n"
print $str
```
