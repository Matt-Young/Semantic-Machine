SRC=netio.c binds.c engine.c filters.c gfun.c \
tables.c graphs.c names.c console.c \
parser.c sqlwraps.c square.c qson.c \
../sqlite/sqlite3.c


all: $(SRC)
	gcc  $(SRC)

jcon: console.c jcon.c
	gcc  console.c jcon.c

clean:
	echo "hello"
	echo $(SRC)