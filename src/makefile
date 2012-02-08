CFLAGS=-I /cygwin/usr/local/include
SRC=netio.c binds.c engine.c filters.c gfun.c \
tables.c graphs.c names.c console.c \
parser.c sqlwraps.c square.c sqlson.c \
../sqlite/sqlite3.c



all: $(SRC)
	gcc -lpthread $(SRC)

jcon: console.c jcon.c
	gcc  console.c jcon.c

clean:
	echo "hello"
	echo $(SRC)