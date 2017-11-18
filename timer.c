#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct time_int {
  int h, m, s;
};

static void do_timer(void);
static void wait_key_input(char *buf);
static int quit_match(char *str);
static struct time_int format(int diff);
static void print_time(struct time_int time_i);
static void alart();

#define P_MAX 2

time_t now, fire;

int main(int argc, char *argv[]) {

  if (argc < 2) {

    fprintf(stderr, "Usage: timer [-H][-M][-S]\n");
    exit(1);
  }
  
  now = time(NULL);
  fire = time(NULL);

  int *sec;
  sec = malloc(10);

  int opt;
  while ((opt = getopt(argc, argv, "M:H:S:v")) != -1) {
    switch (opt) {

    case 'M':
    
      *sec += 60 * atoi(optarg);
      break;

    case 'H':

      *sec += 60 * 60 * atoi(optarg);
      break;

    case 'S':

      *sec += atoi(optarg);
      break;

    case 'v':
      printf("1.0.0\n");
      exit(0);

    case '?':
      fprintf(stderr, "Usage: timer [-H][-M][-S]\n");
      exit(1);
    }
  }
  if (optind < 2) {
    fprintf(stderr, "Usage: timer [-H][-M][-S]\n");
    exit(1);
  }

  fire += *sec;
  free(sec);

  int pid[P_MAX];
  int i;
  for (i = 0; i < P_MAX && (pid[i] = fork()) > 0; i++);

  if (pid[0] == 0) {

    do_timer();
    alart();

  } else if (pid[1] == 0) {

    char buf[256];
    for (;;) {
      wait_key_input(buf);
      if (quit_match(buf) < 0) {
        printf("not [q]\n");
        continue;
      } else {
        exit(0);
      }
    }
  } else {
      
    int status;

    wait(&status);

    for (int i = 0; i < sizeof pid; i++) {
      kill(pid[i], SIGINT);
    }

    if (WIFEXITED(status)) {
    } else if (WIFSIGNALED(status)) {
      printf("signal, sig=%d\n", WTERMSIG(status));
    } else {
      printf("abnormal exit\n");
      exit(0);
    }
  }
}

static void do_timer(void) {

  for (; now < fire; now = time(NULL)) {
    usleep((float)0.25f * 1000000);
    now = time(NULL);
    int diff = fire - now;
    struct time_int time_i;
    time_i = format(diff);
    print_time(time_i);
  }
}

static void wait_key_input(char *buf) {

  if (!fgets(buf, sizeof buf, stdin))
    exit(1);
}

static int quit_match(char *str) {
  if ((strcmp(str, "q\n") == 0)) {
    return 0;
  } else {
    return -1;
  }
}

static struct time_int format(int diff) {
  int h, m, s;
  struct time_int time_i;

  m = diff / 60 % 60;
  s = diff % 60;
  h = diff / 60 / 60;

  time_i.h = h;
  time_i.m = m;
  time_i.s = s;

  return time_i;
}

static void print_time(struct time_int time_i) {
  char *str;
  char h[8];
  char m[4];
  char s[4];

  str = malloc(64);
  snprintf(h, sizeof h, "%i", time_i.h);
  snprintf(m, sizeof m, "%i", time_i.m);
  snprintf(s, sizeof s, "%i", time_i.s);

  strcpy(str, h);
  strcat(str, ":");
    
  if (time_i.m < 10) {
    strcat(str, "0");
    strcat(str, m);
  } else {
    strcat(str, m);
  }
    
  strcat(str, ":");

  if (time_i.s < 10) {
    strcat(str, "0");
    strcat(str, s);
  } else {
    strcat(str, s);
  }

  printf("\r%s ", str);
  fflush(stdout);

  free(str);
}

static void alart() {
  for (int i = 0; i < 100; i++) {
    usleep((float)0.5f * 1000000);
    printf("\007");
    fflush(stdout);
  }
  printf("\n");
}
