CC = gcc
CCFLAGS = -std=c99 -g -Wall -Wextra -lreadline -lssl -lcrypto
EXEC = finddup
OBJS = finddup.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f ${EXEC} ${OBJS}
