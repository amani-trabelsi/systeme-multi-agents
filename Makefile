all: train viewer

train: monde.o politique.o reinforce.o episode.o main_train.o
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g monde.o politique.o reinforce.o episode.o main_train.o -o train -lm -lpthread

viewer: monde.o politique.o reinforce.o episode.o rendu.o main_sdl.o
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g monde.o politique.o reinforce.o episode.o rendu.o main_sdl.o -o viewer -lSDL2 -lSDL2_image -lm

monde.o: monde.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c monde.c

politique.o: politique.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c politique.c

reinforce.o: reinforce.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c reinforce.c

episode.o: episode.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c episode.c

rendu.o: rendu.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c rendu.c

main_train.o: main_train.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c main_train.c

main_sdl.o: main_sdl.c
	gcc -O2 -std=c11 -Wall -Wextra -Werror -g -c main_sdl.c

clean:
	rm -f *.o train viewer 