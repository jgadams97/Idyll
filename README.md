IdyllicBASIC is a variant of BASIC that encapsulates memory management.

This means two things:
	1. Idyllic scripts can be executed from program memory without loading them into RAM.
	2. Idyllic scripts can store its variables within external memory devices. 
	
Both of these things make Idyllic ideal for low-memory applications. The ATMega328P microcontroller, for example, has 2 KB of RAM. Normally, this would be not be enough to run BASIC scripts, as you could only fit a few lines of code into RAM.

However, Idyllic can make use of external RAMs and program memory. The BASIC program does not need to be loaded into RAM to run, and all variables could be stored in an external RAM. The 23LC1024, for example, can store 1 megabit of RAM, which can be used for variable storage with Idyllic. Idyllic stores variables using 32-bit pointers, so it can handle external memory devices up to 4.2 gigabytes. Idyllic also stores these variables using AVL trees, so lookup times are fast.

Variables in Idyllic can be declared with the DIM command.

	dim myVar
	dim $myVar[15]
	
There are three main data types in Idyllic:
	1. Numbers.
	2. Strings.
	3. Addresses.
	
Only numbers and strings can be directly manipulated by the user.
Strings begin with a "$" character.
Addresses begin "@".
Numbers begin with nothing.

Values can be assigned to variables with the equal sign.

	myVar = 5.2
	$myVar = "Hello, World!"
	
You can also assign values on the same line you declare the variable.

	dim myVar = 5.2
	dim $myVar[15] = "Hello, World!"
	
Strings must be provided a size when declaring them.
In the case above, the size of the string is 15.

Trying to store a string into a string variable larger than it was
declared as will result in an out of bounds error.

You cannot "dim" an address. Addresses are created like so:

	@myAddress
	
The address automatically gets assigned its value, which depends
on where it occurs in the file. 

Addresses can be jumped to using "goto @myAddress".
This can be used for looping.

	dim x = 0
	@loop
		print x
		x = x + 1
	if x < 10 then
		goto @loop
	fi
	
This will print numbers 0 through 9.

The IF command can be used for conditional statements.
The block of code between IF and FI will get executed
only if the condition between IF and "then" is true.

The format of an IF command is:

	if LHS comparator RHS then
		...
	fi

Valid comparators are:
	==	Equality.
	>=	Greater than or equal to.
	<=	LEss than or equal to.
	>	Greater than.
	<	Less than.
	!=	Not equal to.

Only "==" and "!=" are valid for string comparisons.
String comparisons only apply to the first 8 characters of the string.

Some commands output values.
These values can be redirected into a variable using the "=>" symbol.

	dim $name[20]
	read => $name
	print "Hello, " + $name + "!"


