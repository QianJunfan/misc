#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

char *flags[] = {"DOING", "DONE", NULL};

#define DOING 0
#define DONE  1
#define MAX_FLOW 5

struct item {
	int f;
	char *s;
	int l;
	struct item *next;
	struct item *prev;
};

struct {
	struct item *items;
	struct item *cur;
	int top;
	int r;
} rt;

union arg {
	char c;
	char *s;
	int i;
};

struct key {
	char mod;
	char key;
	void (*func[MAX_FLOW])(union arg);
	union arg args[MAX_FLOW];
};

void add(union arg a);
void del(union arg a);
void alt(union arg a);
void mod(union arg a);
void mov(union arg a);
void nav(union arg a);
void save(union arg a);
void init(void);
void exec(char m, char k);
void clear(void);
void raw(int i);
void prt(void);
char *input(char *o);
void handler(int s);
int chk_kids(struct item *p);
void fix_down(struct item *p);
void fix_up(struct item *p);

struct key keys[] = {
	{0, 'j',       {mov, NULL}, {{.i = 1}}},
	{0, 'k',       {mov, NULL}, {{.i = -1}}},
	{0, 'h',       {nav, NULL}, {{.i = -1}}},
	{0, 'l',       {nav, NULL}, {{.i = 1}}},
	{0, 32,        {alt, NULL}, {{.i = 0}}},
	{0, 10,        {add, NULL}, {{.i = 0}}},
	{0, 9,         {add, NULL}, {{.i = 1}}},
	{0, 'm',       {mod, NULL}, {{.i = 0}}},
	{0, 127,       {del, NULL}, {{.i = 0}}},
	{0, 0,         {NULL},      {{0}}}
};

int main(void)
{
	char c;
	init();
	signal(SIGINT, handler);
	printf("\033[?25l");
	while (1) {
		clear();
		prt();
		raw(1);
		c = getchar();
		raw(0);
		if (c == 'q') {
			save((union arg){0});
			break;
		}
		exec(0, c);
	}
	printf("\033[?25h");
	return 0;
}

void handler(int s)
{
	save((union arg){0});
	raw(0);
	printf("\033[?25h\nSaved.\n");
	exit(0);
}

void init(void)
{
	struct item *s = malloc(sizeof(struct item));
	struct item *e = malloc(sizeof(struct item));
	FILE *f = fopen(".todosave", "r");
	char b[2048];
	int fl, lv;

	s->next = e; 
        s->prev = NULL;
        s->l = -1;
        s->s = NULL;
	e->prev = s;
        e->next = NULL;
        e->l = -1;
        e->s = NULL;
	rt.items = s;
        rt.cur = s;
        rt.top = 0;

	if (f) {
		while (fscanf(f, "%d\t%d\t", &fl, &lv) == 2) {
			if (fgets(b, sizeof(b), f)) {
				b[strcspn(b, "\n")] = 0;
				union arg a = {.s = b};
				add(a);
				rt.cur->f = fl;
				rt.cur->l = lv;
			}
		}
		fclose(f);
	}
}

void prt(void)
{
	struct item *s = rt.items;
	struct winsize ws;
	int h, i = 0, n = 0, cur_idx = 0;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	h = ws.ws_row;

	if (rt.cur->l == -1) {
		while (rt.cur->l == -1 && rt.cur->next)
			rt.cur = rt.cur->next;
	}

	for (struct item *t = rt.items; t; t = t->next) {
		if (t->l != -1) {
			if (t == rt.cur)
				cur_idx = n;
			n++;
		}
	}

	if (cur_idx < rt.top)
		rt.top = cur_idx;
	if (cur_idx >= rt.top + h)
		rt.top = cur_idx - h + 1;

	printf("\033[H");
	for (n = 0; s; s = s->next) {
		if (s->l == -1)
			continue;

		if (n >= rt.top && i < h) {
			if (s == rt.cur) {
				printf("\033[7m");
				rt.r = i + 1;
			}

			if (s->f == DONE)
				printf("\033[2;9m");

			printf("%*s[%s] %s\033[0m\033[K\n",
			       s->l * 8, "", flags[s->f], s->s ? s->s : "");
			i++;
		}
		n++;
	}

	printf("\033[J");
}


void exec(char m, char k)
{
	struct key *kp;
	int i;
	for (kp = keys; kp->func[0] != NULL; ++kp) {
		if (kp->mod == m && kp->key == k) {
			for (i = 0; i < MAX_FLOW && kp->func[i]; ++i)
				kp->func[i](kp->args[i]);
			return;
		}
	}
}

void fix_up(struct item *p)
{
	struct item *s = p->prev;
	while (s && s->l >= p->l)
                s = s->prev;
	if (s && s->l != -1 && chk_kids(s)) {
		s->f = DONE;
                fix_up(s);
	}
}

void fix_down(struct item *p)
{
        struct item *s = p->prev;

        while (s && s->l >= p->l)
                s = s->prev;

        if (s && s->l != -1) {
                if (s->f == DONE) {
                        s->f = DOING;
                        fix_down(s);
                }
        }
}

void add(union arg a)
{
        struct item *p = rt.cur, *t = p;
        struct item *n = malloc(sizeof(struct item));
        char *res;

        if (a.i == 0 && p->l != -1) {
                while (t->next && t->next->l > p->l)
                        t = t->next;
        }

        n->next = t->next;
        n->prev = t;

        if (t->next)
                t->next->prev = n;

        t->next = n;

        n->f = DOING;
        n->l = (p->l == -1) ? 0 : p->l + a.i;

        if (n->l < 0)
                n->l = 0;
        n->s = NULL;

        if ((unsigned long)a.s < 4096) {
                rt.cur = n;
                clear();
                prt();
                printf("\033[%d;%dH", rt.r, (n->l * 8) + 9);
                res = input(NULL);

                if (!res || !*res) {
                        t->next = n->next;
                        if (n->next)
                                n->next->prev = t;
                        free(n);
                        rt.cur = p;
                        return;
                }

                n->s = res;
                fix_down(n);
        } else {
                n->s = strdup(a.s);
                rt.cur = n;
        }
}

void mod(union arg a)
{
	struct item *p = rt.cur;
	char *n;

	if (!p || p->l == -1)
                return;

	clear();
	prt();
	printf("\033[%d;%dH", rt.r, (p->l * 8) + 9);

	n = input(p->s);

	if (n && *n) {
		free(p->s);
		p->s = n;
	} else if (n) {
		free(n);
	}
}
void del_kids(struct item *p)
{
	struct item *s = p->next;
	while (s && s->l > p->l) {
		struct item *tmp = s;
		s = s->next;
		if (s) s->prev = p;
		p->next = s;
		free(tmp->s);
		free(tmp);
	}
}

void del(union arg a)
{
	struct item *p = rt.cur;
	struct item *n;

	if (!p || p->l == -1)
		return;

	del_kids(p);

	n = (p->prev && p->prev->l != -1) ? p->prev : p->next;
	
	if (p->next)
		p->next->prev = p->prev;
	if (p->prev)
		p->prev->next = p->next;

	free(p->s);
	free(p);

	if (n && n->l != -1) {
		rt.cur = n;
	} else if (n && n->next && n->next->l != -1) {
		rt.cur = n->next;
	} else {
		struct item *t = rt.items;
		while (t && t->l == -1) t = t->next;
		rt.cur = t ? t : rt.items;
	}
}

int chk_kids(struct item *p)
{
	struct item *s = p->next;
	while (s && s->l > p->l) {
		if (s->f == DOING)
                        return 0;
		s = s->next;
	}
	return 1;
}



void alt(union arg a)
{
	if (!rt.cur || rt.cur->l == -1)
                return;

	if (rt.cur->f == DOING) {
		if (!chk_kids(rt.cur))
                        return;

		rt.cur->f = DONE;
                fix_up(rt.cur);
	} else {
		rt.cur->f = DOING; 
                fix_down(rt.cur);
	}
}

void mov(union arg a)
{
	struct item *p = rt.cur;
	int n = a.i;
	while (n != 0 && p) {
		if (n > 0) {
			if (p->next && p->next->l != -1) {
                                p = p->next;
                                n--;
                        }
			else break;
		} else {
			if (p->prev && p->prev->l != -1) { 
                                p = p->prev; 
                                n++;
                        }
			else break;
		}
	}
	rt.cur = p;
}

void nav(union arg a)
{
	struct item *p = rt.cur;
	struct item *t;
	if (!p || p->l == -1)
                return;

	t = (a.i < 0) ? p->prev : p->next;

	while (t && t->l != -1) {
		if (a.i < 0 && t->l < p->l) {
			rt.cur = t;
			return;
		}
		if (a.i > 0 && t->l > p->l) {
			rt.cur = t;
			return;
		}

		t = (a.i < 0) ? t->prev : t->next;
	}
}


char *input(char *o)
{
	char b[1024] = {0};
	int i = 0;
	char c;

	printf("\033[?25h\033[4m");

	if (o) {
		strncpy(b, o, 1023);
		i = strlen(b);
		printf("%s", b);
	}

	raw(1);

	while (1) {
		c = getchar();
		if (c == 10 || c == 13) 
                        break;

		if (c == 127 || c == 8) {
			if (i > 0) { 
                                i--;
                                b[i] = 0;
                                printf("\b \b");
                        }
		} else if (i < 1023) {
			b[i++] = c; putchar(c);
		}
	}

	raw(0);
	printf("\033[0m\033[?25l");
	return strdup(b);
}

void save(union arg a)
{
	FILE *f = fopen(".todosave", "w");
	struct item *s = rt.items;

	if (!f) 
                return;

	while (s) {
		if (s->l != -1 && s->s)
			fprintf(f, "%d\t%d\t%s\n", s->f, s->l, s->s);
		s = s->next;
	}
	fclose(f);
}

void clear(void)
{
	printf("\033[2J\033[H");
	fflush(stdout);
}

void raw(int i)
{
	static struct termios old;
	struct termios new;

	if (i) {
		tcgetattr(STDIN_FILENO, &old);
		new = old;
		new.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &new);
	} else {
                tcsetattr(STDIN_FILENO, TCSANOW, &old);
        }
}
