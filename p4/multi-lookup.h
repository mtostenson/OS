#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "queue.h"

#define MAX_INPUT_FILES 10
#define THREAD_MAX 5
#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define MAX_NAME_LENGTH 1025
#define INPUTFS "%1024s"

queue q;