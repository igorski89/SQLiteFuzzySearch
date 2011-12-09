## About ##

A set of `SQLite 3` extensions that allow perform a approximate string searching.


## Compiling ##

In order to compile this extensions You need an `gcc` or `clang` (just install Xcode 4.2) and `sqlite` library installed (comes bundled with OS X but installed from MacPorts or Homebrew is ok).

To compile an extension using a command-line:
	clang -bundle -fPIC -Isqlite3 -o <desired extension name>.sqlext <filename>.c

For example, to compile a extension that uses Levenshtein distance:
	clang -bundle -fPIC -Isqlite3 -o levenshtein.sqlext levenshtein.c


## Usage ##

### Loading extension ###

In order to use extension you need to load it. You can do this by calling:
	.load <path to extension file>

For example, if you want to load a `Soundex` extension, you need to call:
	.load soundex.sqlext
Note that you should start `sqlite` interactive shell in the same directory that contains `soundex.sqlext` file. Otherwise you need to specify full path.

### Using functions in SQL query ###
Basically these extensions just define a number of functions that could be used in `SQL` query.

For example, if you want to fetch all items that have a name similar to `orange` via `Soundex` algorithm, you should use this query:
	SELECT * FROM items WHERE SOUNDEX(name) == SOUNDEX('orange')


If you want to fetch all peoples with name that has a Levenshtein dispense less than 3 with word "John" you should use this query:
	SELECT * FROM peoples WHERE LEVENSHTEIN(name,'John',3)

