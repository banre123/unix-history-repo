/* $Header: fortune.c,v 1.40 88/08/18 11:41:50 arnold Exp $ */

# include	<stdio.h>
# include	<assert.h>
# include	<sys/param.h>
# include	<sys/types.h>
# include	<sys/stat.h>
# include	<sys/dir.h>
# include	"strfile.h"
# include	"random.h"

#ifdef	SYSV
# include	<dirent.h>

# define	NO_LOCK
# define	REGCMP
# ifdef	NO_REGEX
#	undef	 NO_REGEX
# endif	/* NO_REGEX */
# define	index	strchr
# define	rindex	strrchr
#endif	/* SYSV */

#ifndef NO_REGEX
# include	<ctype.h>
#endif	/* NO_REGEX */

# ifndef NO_LOCK
# include	<sys/file.h>
# endif	/* NO_LOCK */

# ifndef F_OK
/* codes for access() */
# define	F_OK		0	/* does file exist */
# define	X_OK		1	/* is it executable by caller */
# define	W_OK		2	/* writable by caller */
# define	R_OK		4	/* readable by caller */
# endif	/* F_OK */

# define	TRUE	1
# define	FALSE	0
# define	bool	short

# define	MINW	6		/* minimum wait if desired */
# define	CPERS	20		/* # of chars for each sec */
# define	SLEN	160		/* # of chars in short fortune */

# define	POS_UNKNOWN	((unsigned long) -1)	/* pos for file unknown */
# define	NO_PROB		(-1)		/* no prob specified for file */

# ifndef FORTDIR
# define	FORTDIR		"/usr/games/lib/fortune"
# endif	/* FORTDIR */

# ifdef DEBUG
# define	DPRINTF(l,x)	if (Debug >= l) fprintf x; else
# undef		NDEBUG
# else	/* DEBUG */
# define	DPRINTF(l,x)
# define	NDEBUG	1
# endif	/* DEBUG */

typedef struct fd {
	int		percent;
	int		fd, datfd;
	unsigned long	pos;
	FILE		*inf;
	char		*name;
	char		*path;
	char		*datfile, *posfile;
	bool		read_tbl;
	bool		was_pos_file;
	STRFILE		tbl;
	int		num_children;
	struct fd	*child, *parent;
	struct fd	*next, *prev;
} FILEDESC;

bool	Found_one;			/* did we find a match? */
bool	Find_files	= FALSE;	/* just find a list of proper fortune files */
bool	Wait		= FALSE;	/* wait desired after fortune */
bool	Short_only	= FALSE;	/* short fortune desired */
bool	Long_only	= FALSE;	/* long fortune desired */
bool	Offend		= FALSE;	/* offensive fortunes only */
bool	All_forts	= FALSE;	/* any fortune allowed */
bool	Equal_probs	= FALSE;	/* scatter un-allocted prob equally */
#ifndef NO_REGEX
bool	Match		= FALSE;	/* dump fortunes matching a pattern */
#endif
#ifdef DEBUG
bool	Debug = FALSE;			/* print debug messages */
#endif

char	*Fortbuf = NULL;			/* fortune buffer for -m */
char	*Usage[]	= {
# ifdef	NO_REGEX
       "usage:  fortune [ - ] [ -fwsloa ] [ [N%] file/dir/\"all\" ]",
# else	/* NO_REGEX */
       "usage:  fortune [ - ] [ -fwsloai ] [ -m pattern ] [ [N%] file/dir/\"all\" ]",
# endif	/* NO_REGEX */
       "	- - give this summary of usage",
       "	f - list fortune files",
       "	w - wait after printing message in order to give time to read",
       "	s - short fortunes only",
       "	l - long fortunes only",
       "	o - offensive fortunes only",
       "	a - any fortune, regular or offensive",
# ifndef NO_REGEX
       "	m - print fortunes which match a pattern",
       "	i - ignore case in matching patterns",
# endif	/* NO_REGEX */
       "		Mail suggested fortunes to \"fortune@berkeley.edu\"",
	NULL
	};

int	Fort_len = 0;

off_t	Seekpts[2];			/* seek pointers to fortunes */

FILEDESC	*File_list = NULL,	/* Head of file list */
		*File_tail = NULL;	/* Tail of file list */
FILEDESC	*Fortfile;		/* Fortune file to use */

STRFILE		Noprob_tbl;		/* sum of data for all no prob files */

char	*do_malloc(), *copy(), *off_name();

FILEDESC	*pick_child(), *new_fp();

extern char	*malloc(), *index(), *rindex(), *strcpy(), *strcat();

extern time_t	time();

#ifndef NO_REGEX
char	*conv_pat();
#endif

#ifndef NO_REGEX
#ifdef REGCMP
# define	RE_COMP(p)	(Re_pat = regcmp(p, NULL))
# define	BAD_COMP(f)	((f) == NULL)
# define	RE_EXEC(p)	regex(Re_pat, (p))

char	*Re_pat;

char	*regcmp(), *regex();
#else
# define	RE_COMP(p)	(p = re_comp(p))
# define	BAD_COMP(f)	((f) != NULL)
# define	RE_EXEC(p)	re_exec(p)

char	*re_comp();
#ifdef SYSV
char	*re_exec();
#else
int	re_exec();
#endif
#endif
#endif

main(ac, av)
int	ac;
char	*av[];
{
	int	fd;
	char	line[BUFSIZ];

	getargs(ac, av);

#ifndef NO_REGEX
	if (Match)
		exit(find_matches() != 0);
#endif

	init_prob();
	srnd((long) (time((time_t *) NULL) + getpid()));
	do {
		get_fort();
	} while ((Short_only && fortlen() > SLEN) ||
		 (Long_only && fortlen() <= SLEN));

	open_fp(Fortfile);
	(void) fseek(Fortfile->inf, Seekpts[0], 0);
	for (Fort_len = 0; fgets(line, sizeof line, Fortfile->inf) != NULL &&
			   !STR_ENDSTRING(line, Fortfile->tbl);
	     Fort_len++)
		fputs(line, stdout);
	(void) fflush(stdout);

	if ((fd = creat(Fortfile->posfile, 0666)) < 0) {
		perror(Fortfile->posfile);
		exit(1);
	}
#ifdef	LOCK_EX
	/*
	 * if we can, we exclusive lock, but since it isn't very
	 * important, we just punt if we don't have easy locking
	 * available.
	 */
	(void) flock(fd, LOCK_EX);
#endif	/* LOCK_EX */
	write(fd, (char *) &Fortfile->pos, sizeof Fortfile->pos);
	if (!Fortfile->was_pos_file)
		(void) chmod(Fortfile->path, 0666);
#ifdef	LOCK_EX
	(void) flock(fd, LOCK_UN);
#endif	/* LOCK_EX */
	if (Wait) {
		if (Fort_len == 0)
			(void) fortlen();
		sleep((unsigned int) max(Fort_len / CPERS, MINW));
	}
	exit(0);
	/* NOTREACHED */
}

/*
 * fortlen:
 *	Return the length of the fortune.
 */
fortlen()
{
	register int	nchar;
	char		line[BUFSIZ];

	if (!(Fortfile->tbl.str_flags & (STR_RANDOM | STR_ORDERED)))
		nchar = (Seekpts[1] - Seekpts[0] <= SLEN);
	else {
		open_fp(Fortfile);
		(void) fseek(Fortfile->inf, Seekpts[0], 0);
		nchar = 0;
		while (fgets(line, sizeof line, Fortfile->inf) != NULL &&
		       !STR_ENDSTRING(line, Fortfile->tbl))
			nchar += strlen(line);
	}
	Fort_len = nchar;
	return nchar;
}

/*
 *	This routine evaluates the arguments on the command line
 */
getargs(ac, av)
register int	ac;
register char	*av[];
{
	register int	i;
	register char	*sp;
	register int	j;
	register short	bad;
	register int	ignore_case;
# ifndef NO_REGEX
	register char	*pat;
# endif	/* NO_REGEX */

	ignore_case = FALSE;
	bad = FALSE;
	pat = NULL;
	for (i = 1; i < ac; i++)  {
		if (av[i][0] != '-')
			break;
		else if (av[i][1] == '\0') {
			j = 0;
			while (Usage[j] != NULL)
				puts(Usage[j++]);
			exit(0);
			/* NOTREACHED */
		}
		else
			for (sp = &av[i][1]; *sp != '\0'; sp++)
				switch (*sp) {
				  case 'f':	/* find fortune files */
					Find_files++;
					break;
				  case 'w':	/* give time to read */
					Wait++;
					break;
				  case 's':	/* short ones only */
					Short_only++;
					Long_only = FALSE;
					break;
				  case 'l':	/* long ones only */
					Long_only++;
					Short_only = FALSE;
					break;
				  case 'o':	/* offensive ones only */
					Offend++;
					break;
				  case 'a':	/* any fortune */
					All_forts++;
					break;
				  case 'm':	/* dump out the fortunes */
# ifdef	NO_REGEX
				  case 'i':
					fprintf(stderr,
						"can't match fortunes on this system (Sorry)\n");
					bad++;
# else	/* NO_REGEX */
					Match++;
					if (sp[1]) {
						pat = ++sp;
						while (*sp)
							sp++;
					}
					else if (i + 1 < ac)
						pat = av[++i];
					else {
						fprintf(stderr,
							"must give pattern\n");
						bad++;
						break;
					}
					break;
				  case 'i':
					ignore_case++;
					break;
				  case 'e':
					Equal_probs++;
					break;
# ifdef DEBUG
				  case 'D':
					Debug++;
					break;
# endif /* DEBUG */
# endif	/* NO_REGEX */
				  default:
					fprintf(stderr, "unknown flag: '%c'\n",
						*sp);
					bad++;
					break;
				}
	}
	if (!bad && !form_file_list(&av[i], ac - i))
		exit(1);	/* errors printed through form_file_list() */
#ifdef DEBUG
	if (Debug >= 1)
		print_file_list();
#endif /* DEBUG */
	if (Find_files) {
		print_file_list();
		exit(0);
	}

# ifndef NO_REGEX
	if (pat != NULL) {
		if (ignore_case)
			pat = conv_pat(pat);
		if (BAD_COMP(RE_COMP(pat))) {
#ifndef REGCMP
			fprintf(stderr, "%s\n", pat);
#else	/* REGCMP */
			fprintf(stderr, "bad pattern: %s\n", pat);
#endif	/* REGCMP */
			bad++;
		}
	}
# endif	/* NO_REGEX */

	if (bad) {
		printf("use \"%s -\" to get usage\n", av[0]);
		exit(-1);
	}
}

/*
 * form_file_list:
 *	Form the file list from the file specifications.
 */
form_file_list(files, file_cnt)
register char	**files;
register int	file_cnt;
{
	register int	i, percent;
	register char	*sp;

	if (file_cnt == 0)
		if (Find_files)
			return add_file(NO_PROB, FORTDIR, NULL, &File_list,
					&File_tail, NULL);
		else
			return add_file(NO_PROB, "fortunes", FORTDIR,
					&File_list, &File_tail, NULL);
	for (i = 0; i < file_cnt; i++) {
		percent = NO_PROB;
		if (!isdigit(files[i][0]))
			sp = files[i];
		else {
			percent = 0;
			for (sp = files[i]; isdigit(*sp); sp++)
				percent = percent * 10 + *sp - '0';
			if (percent > 100) {
				fprintf(stderr, "percentages must be <= 100\n");
				return FALSE;
			}
			if (*sp == '.') {
				fprintf(stderr, "percentages must be integers\n");
				return FALSE;
			}
			/*
			 * If the number isn't followed by a '%', then
			 * it was not a percentage, just the first part
			 * of a file name which starts with digits.
			 */
			if (*sp != '%') {
				percent = NO_PROB;
				sp = files[i];
			}
			else if (*++sp == '\0') {
				if (++i >= file_cnt) {
					fprintf(stderr, "percentages must precede files\n");
					return FALSE;
				}
				sp = files[i];
			}
		}
		if (strcmp(sp, "all") == 0)
			sp = FORTDIR;
		if (!add_file(percent, sp, NULL, &File_list, &File_tail, NULL))
			return FALSE;
	}
	return TRUE;
}

/*
 * add_file:
 *	Add a file to the file list.
 */
add_file(percent, file, dir, head, tail, parent)
int		percent;
register char	*file;
char		*dir;
FILEDESC	**head, **tail;
FILEDESC	*parent;
{
	register FILEDESC	*fp;
	register int		fd;
	register char		*path, *offensive;
	register int		len;
	register bool		was_malloc;
	register bool		isdir;

	if (dir == NULL) {
		path = file;
		was_malloc = FALSE;
	}
	else {
		path = do_malloc((unsigned int) (strlen(dir) + strlen(file) + 2));
		(void) strcat(strcat(strcpy(path, dir), "/"), file);
		was_malloc = TRUE;
	}
	if ((isdir = is_dir(path)) && parent != NULL) {
		if (was_malloc)
			free(path);
		return FALSE;	/* don't recurse */
	}
	offensive = NULL;
	if (!isdir && parent == NULL && (All_forts || Offend) &&
	    !is_off_name(path)) {
		offensive = off_name(path);
		was_malloc = TRUE;
		if (Offend) {
			if (was_malloc)
				free(path);
			path = offensive;
			file = off_name(file);
		}
	}

	DPRINTF(1, (stderr, "adding file \"%s\"\n", path));
over:
	if ((fd = open(path, 0)) < 0) {
		/*
		 * This is a sneak.  If the user said -a, and if the
		 * file we're given isn't a file, we check to see if
		 * there is a -o version.  If there is, we treat it as
		 * if *that* were the file given.  We only do this for
		 * individual files -- if we're scanning a directory,
		 * we'll pick up the -o file anyway.
		 */
		if (All_forts && offensive != NULL) {
			path = offensive;
			if (was_malloc)
				free(path);
			offensive = NULL;
			was_malloc = TRUE;
			DPRINTF(1, (stderr, "\ttrying \"%s\"\n", path));
			file = off_name(file);
			goto over;
		}
		if (dir == NULL && file[0] != '/')
			return add_file(percent, file, FORTDIR, head, tail,
					parent);
		if (parent == NULL)
			perror(path);
		if (was_malloc)
			free(path);
		return FALSE;
	}

	DPRINTF(2, (stderr, "path = \"%s\"\n", path));

	fp = new_fp();
	fp->fd = fd;
	fp->percent = percent;
	fp->name = file;
	fp->path = path;
	fp->parent = parent;

	if ((isdir && !add_dir(fp)) ||
	    (!isdir &&
	     !is_fortfile(path, &fp->datfile, &fp->posfile, (parent != NULL))))
	{
		if (parent == NULL)
			fprintf(stderr,
				"fortune:%s not a fortune file or directory\n",
				path);
		free((char *) fp);
		if (was_malloc)
			free(path);
		do_free(fp->datfile);
		do_free(fp->posfile);
		do_free(offensive);
		return FALSE;
	}
	/*
	 * If the user said -a, we need to make this node a pointer to
	 * both files, if there are two.  We don't need to do this if
	 * we are scanning a directory, since the scan will pick up the
	 * -o file anyway.
	 */
	if (All_forts && parent == NULL && !is_off_name(path))
		all_forts(fp, offensive);
	if (*head == NULL)
		*head = *tail = fp;
	else if (fp->percent == NO_PROB) {
		(*tail)->next = fp;
		fp->prev = *tail;
		*tail = fp;
	}
	else {
		(*head)->prev = fp;
		fp->next = *head;
		*head = fp;
	}
	fp->was_pos_file = (access(fp->posfile, W_OK) >= 0);

	return TRUE;
}

/*
 * new_fp:
 *	Return a pointer to an initialized new FILEDESC.
 */
FILEDESC *
new_fp()
{
	register FILEDESC	*fp;

	fp = (FILEDESC *) do_malloc(sizeof *fp);
	fp->datfd = -1;
	fp->pos = POS_UNKNOWN;
	fp->inf = NULL;
	fp->fd = -1;
	fp->percent = NO_PROB;
	fp->read_tbl = FALSE;
	fp->next = NULL;
	fp->prev = NULL;
	fp->child = NULL;
	fp->parent = NULL;
	fp->datfile = NULL;
	fp->posfile = NULL;
	return fp;
}

/*
 * off_name:
 *	Return a pointer to the offensive version of a file of this name.
 */
char *
off_name(file)
char	*file;
{
	char	*new;

	new = copy(file, (unsigned int) (strlen(file) + 2));
	return strcat(new, "-o");
}

/*
 * is_off_name:
 *	Is the file an offensive-style name?
 */
is_off_name(file)
char	*file;
{
	int	len;

	len = strlen(file);
	return (len >= 3 && file[len - 2] == '-' && file[len - 1] == 'o');
}

/*
 * all_forts:
 *	Modify a FILEDESC element to be the parent of two children if
 *	there are two children to be a parent of.
 */
all_forts(fp, offensive)
register FILEDESC	*fp;
char			*offensive;
{
	register char		*sp;
	register FILEDESC	*scene, *obscene;
	register int		fd;
	auto char		*datfile, *posfile;

	if (fp->child != NULL)	/* this is a directory, not a file */
		return;
	if (!is_fortfile(offensive, &datfile, &posfile, FALSE))
		return;
	if ((fd = open(offensive, 0)) < 0)
		return;
	DPRINTF(1, (stderr, "adding \"%s\" because of -a\n", offensive));
	scene = new_fp();
	obscene = new_fp();
	*scene = *fp;

	fp->num_children = 2;
	fp->child = scene;
	scene->next = obscene;
	obscene->next = NULL;
	scene->child = obscene->child = NULL;
	scene->parent = obscene->parent = fp;

	fp->fd = -1;
	scene->percent = obscene->percent = NO_PROB;

	obscene->fd = fd;
	obscene->inf = NULL;
	obscene->path = offensive;
	if ((sp = rindex(offensive, '/')) == NULL)
		obscene->name = offensive;
	else
		obscene->name = ++sp;
	obscene->datfile = datfile;
	obscene->posfile = posfile;
	obscene->read_tbl = FALSE;
	obscene->was_pos_file = (access(obscene->posfile, W_OK) >= 0);
}

/*
 * add_dir:
 *	Add the contents of an entire directory.
 */
add_dir(fp)
register FILEDESC	*fp;
{
	register DIR		*dir;
#ifdef SYSV
	register struct dirent	*dirent;	/* NIH, of course! */
#else
	register struct direct	*dirent;
#endif
	auto FILEDESC		*tailp;
	auto char		*name;

	(void) close(fp->fd);
	fp->fd = -1;
	if ((dir = opendir(fp->path)) == NULL) {
		perror(fp->path);
		return FALSE;
	}
	tailp = NULL;
	DPRINTF(1, (stderr, "adding dir \"%s\"\n", fp->path));
	fp->num_children = 0;
	while ((dirent = readdir(dir)) != NULL) {
		if (dirent->d_namlen == 0)
			continue;
		name = copy(dirent->d_name, dirent->d_namlen);
		if (add_file(NO_PROB, name, fp->path, &fp->child, &tailp, fp))
			fp->num_children++;
		else
			free(name);
	}
	if (fp->num_children == 0) {
		fprintf(stderr, "fortune:%s:No fortune files in directory\n", fp->path);
		return FALSE;
	}
	return TRUE;
}

/*
 * is_dir:
 *	Return TRUE if the file is a directory, FALSE otherwise.
 */
is_dir(file)
char	*file;
{
	auto struct stat	sbuf;

	if (stat(file, &sbuf) < 0)
		return FALSE;
	return (sbuf.st_mode & S_IFDIR);
}

/*
 * is_fortfile:
 *	Return TRUE if the file is a fortune database file.  We try and
 *	exclude files without reading them if possible to avoid
 *	overhead.  Files which start with ".", or which have "illegal"
 *	suffixes, as contained in suflist[], are ruled out.
 */
is_fortfile(file, datp, posp, check_for_offend)
char	*file;
char	**datp, **posp;
int	check_for_offend;
{
	register int	i;
	register char	*sp;
	register char	*datfile;
	static char	*suflist[] = {	/* list of "illegal" suffixes" */
				"dat", "pos", "c", "h", "p", "i", "f",
				"pas", "ftn", "ins.c", "ins,pas",
				"ins.ftn", "sml",
				NULL
			};

	DPRINTF(2, (stderr, "is_fortfile(%s) returns ", file));

	/*
	 * Preclude any -o files for offendable people, and any non -o
	 * files for completely offensive people.
	 */
	if (check_for_offend && !All_forts) {
		i = strlen(file);
		if (Offend ^ (file[i - 2] == '-' && file[i - 1] == 'o'))
			return FALSE;
	}

	if ((sp = rindex(file, '/')) == NULL)
		sp = file;
	else
		sp++;
	if (*sp == '.') {
		DPRINTF(2, (stderr, "FALSE (file starts with '.')\n"));
		return FALSE;
	}
	if ((sp = rindex(sp, '.')) != NULL) {
		sp++;
		for (i = 0; suflist[i] != NULL; i++)
			if (strcmp(sp, suflist[i]) == 0) {
				DPRINTF(2, (stderr, "FALSE (file has suffix \".%s\")\n", sp));
				return FALSE;
			}
	}

	datfile = copy(file, (unsigned int) (strlen(file) + 4)); /* +4 for ".dat" */
	strcat(datfile, ".dat");
	if (access(datfile, R_OK) < 0) {
		free(datfile);
		DPRINTF(2, (stderr, "FALSE (no \".dat\" file)\n"));
		return FALSE;
	}
	if (datp != NULL)
		*datp = datfile;
	else
		free(datfile);
	if (posp != NULL) {
		*posp = copy(file, (unsigned int) (strlen(file) + 4)); /* +4 for ".dat" */
		(void) strcat(*posp, ".pos");
	}
	DPRINTF(2, (stderr, "TRUE\n"));
	return TRUE;
}

/*
 * copy:
 *	Return a malloc()'ed copy of the string
 */
char *
copy(str, len)
char		*str;
unsigned int	len;
{
	char	*new, *sp;

	new = do_malloc(len + 1);
	sp = new;
	do {
		*sp++ = *str;
	} while (*str++);
	return new;
}

/*
 * do_malloc:
 *	Do a malloc, checking for NULL return.
 */
char *
do_malloc(size)
unsigned int	size;
{
	char	*new;

	if ((new = malloc(size)) == NULL) {
		fprintf(stderr, "fortune:Out of space\n");
		exit(1);
	}
	return new;
}

/*
 * do_free:
 *	Free malloc'ed space, if any.
 */
do_free(ptr)
char	*ptr;
{
	if (ptr != NULL)
		free(ptr);
}

/*
 * init_prob:
 *	Initialize the fortune probabilities.
 */
init_prob()
{
	register FILEDESC	*fp, *last;
	register int		percent, num_noprob, frac;

	/*
	 * Distribute the residual probability (if any) across all
	 * files with unspecified probability (i.e., probability of 0)
	 * (if any).
	 */

	percent = 0;
	num_noprob = 0;
	for (fp = File_tail; fp != NULL; fp = fp->prev)
		if (fp->percent == NO_PROB) {
			num_noprob++;
			if (Equal_probs)
				last = fp;
		}
		else
			percent += fp->percent;
	DPRINTF(1, (stderr, "summing probabilities:%d%% with %d NO_PROB's",
		    percent, num_noprob));
	if (percent > 100) {
		fprintf(stderr, "fortune:Probabilities sum to %d%%!\n",
			percent);
		exit(1);
	}
	else if (percent < 100 && num_noprob == 0) {
		fprintf(stderr,
			"fortune:No place to put residual probability (%d%%)\n",
			percent);
		exit(1);
	}
	else if (percent == 100 && num_noprob != 0) {
		fprintf(stderr,
			"fortune:No probability left to put in residual files\n",
			percent);
		exit(1);
	}
	percent = 100 - percent;
	if (Equal_probs)
		if (num_noprob != 0) {
			if (num_noprob > 1) {
				frac = percent / num_noprob;
				DPRINTF(1, (stderr, ", frac = %d%%", frac));
				for (fp = File_list; fp != last; fp = fp->next)
					if (fp->percent == NO_PROB) {
						fp->percent = frac;
						percent -= frac;
					}
			}
			last->percent = percent;
			DPRINTF(1, (stderr, ", residual = %d%%", percent));
		}
	else {
		DPRINTF(1, (stderr,
			    ", %d%% distributed over remaining fortunes\n",
			    percent));
	}
	DPRINTF(1, (stderr, "\n"));

#ifdef DEBUG
	if (Debug >= 1)
		print_file_list();
#endif
}

/*
 * get_fort:
 *	Get the fortune data file's seek pointer for the next fortune.
 */
get_fort()
{
	register FILEDESC	*fp;
	register int		choice;

	if (File_list->next == NULL || File_list->percent == NO_PROB)
		fp = File_list;
	else {
		choice = rnd(100L);
		DPRINTF(1, (stderr, "choice = %d\n", choice));
		for (fp = File_list; fp->percent != NO_PROB; fp = fp->next)
			if (choice < fp->percent)
				break;
			else {
				choice -= fp->percent;
				DPRINTF(1, (stderr,
					    "    skip \"%s\", %d%% (choice = %d)\n",
					    fp->name, fp->percent, choice));
			}
			DPRINTF(1, (stderr,
				    "using \"%s\", %d%% (choice = %d)\n",
				    fp->name, fp->percent, choice));
	}
	if (fp->percent != NO_PROB)
		get_tbl(fp);
	else {
		if (fp->next != NULL) {
			sum_noprobs(fp);
			choice = rnd((long) Noprob_tbl.str_numstr);
			DPRINTF(1, (stderr, "choice = %d (of %d) \n", choice,
				    Noprob_tbl.str_numstr));
			while (choice >= fp->tbl.str_numstr) {
				choice -= fp->tbl.str_numstr;
				fp = fp->next;
				DPRINTF(1, (stderr,
					    "    skip \"%s\", %d (choice = %d)\n",
					    fp->name, fp->tbl.str_numstr,
					    choice));
			}
			DPRINTF(1, (stderr, "using \"%s\", %d\n", fp->name,
				    fp->tbl.str_numstr));
		}
		get_tbl(fp);
	}
	if (fp->child != NULL) {
		DPRINTF(1, (stderr, "picking child\n"));
		fp = pick_child(fp);
	}
	Fortfile = fp;
	get_pos(fp);
	open_dat(fp);
	(void) lseek(fp->datfd,
		     (off_t) (sizeof fp->tbl + fp->pos * sizeof Seekpts[0]), 0);
	read(fp->datfd, Seekpts, sizeof Seekpts);
}

/*
 * pick_child
 *	Pick a child from a chosen parent.
 */
FILEDESC *
pick_child(parent)
FILEDESC	*parent;
{
	register FILEDESC	*fp;
	register int		choice;

	if (Equal_probs) {
		choice = rnd((long) parent->num_children);
		DPRINTF(1, (stderr, "    choice = %d (of %d)\n",
			    choice, parent->num_children));
		for (fp = parent->child; choice--; fp = fp->next)
			continue;
		DPRINTF(1, (stderr, "    using %s\n", fp->name));
		return fp;
	}
	else {
		get_tbl(parent);
		choice = rnd((long) parent->tbl.str_numstr);
		DPRINTF(1, (stderr, "    choice = %d (of %d)\n",
			    choice, parent->tbl.str_numstr));
		for (fp = parent->child; choice >= fp->tbl.str_numstr;
		     fp = fp->next) {
			choice -= fp->tbl.str_numstr;
			DPRINTF(1, (stderr, "\tskip %s, %d (choice = %d)\n",
				    fp->name, fp->tbl.str_numstr, choice));
		}
		DPRINTF(1, (stderr, "    using %s, %d\n", fp->name,
			    fp->tbl.str_numstr));
		return fp;
	}
}

/*
 * sum_noprobs:
 *	Sum up all the noprob probabilities, starting with fp.
 */
sum_noprobs(fp)
register FILEDESC	*fp;
{
	static bool	did_noprobs = FALSE;

	if (did_noprobs)
		return;
	zero_tbl(&Noprob_tbl);
	while (fp != NULL) {
		get_tbl(fp);
		sum_tbl(&Noprob_tbl, &fp->tbl);
		fp = fp->next;
	}
	did_noprobs = TRUE;
}

max(i, j)
register int	i, j;
{
	return (i >= j ? i : j);
}

/*
 * open_fp:
 *	Assocatiate a FILE * with the given FILEDESC.
 */
open_fp(fp)
FILEDESC	*fp;
{
	if (fp->inf == NULL && (fp->inf = fdopen(fp->fd, "r")) == NULL) {
		perror(fp->path);
		exit(1);
	}
}

/*
 * open_dat:
 *	Open up the dat file if we need to.
 */
open_dat(fp)
FILEDESC	*fp;
{
	if (fp->datfd < 0 && (fp->datfd = open(fp->datfile, 0)) < 0) {
		perror(fp->datfile);
		exit(1);
	}
}

/*
 * get_pos:
 *	Get the position from the pos file, if there is one.  If not,
 *	return a random number.
 */
get_pos(fp)
FILEDESC	*fp;
{
	int	fd;

	assert(fp->read_tbl);
	if (fp->pos == POS_UNKNOWN) {
		if ((fd = open(fp->posfile, 0)) < 0 ||
		    read(fd, &fp->pos, sizeof fp->pos) != sizeof fp->pos)
			fp->pos = rnd(fp->tbl.str_numstr);
		else if (fp->pos >= fp->tbl.str_numstr)
			fp->pos %= fp->tbl.str_numstr;
		if (fd >= 0)
			(void) close(fd);
	}
	if (++(fp->pos) >= fp->tbl.str_numstr)
		fp->pos -= fp->tbl.str_numstr;
	DPRINTF(1, (stderr, "pos for %s is %d\n", fp->name, fp->pos));
}

/*
 * get_tbl:
 *	Get the tbl data file the datfile.
 */
get_tbl(fp)
FILEDESC	*fp;
{
	auto int		fd;
	register FILEDESC	*child;

	if (fp->read_tbl)
		return;
	if (fp->child == NULL) {
		if ((fd = open(fp->datfile, 0)) < 0) {
			perror(fp->datfile);
			exit(1);
		}
		if (read(fd, (char *) &fp->tbl, sizeof fp->tbl) != sizeof fp->tbl) {
			fprintf(stderr, "fortune:%s corrupted\n", fp->path);
			exit(1);
		}
		(void) close(fd);
	}
	else {
		zero_tbl(&fp->tbl);
		for (child = fp->child; child != NULL; child = child->next) {
			get_tbl(child);
			sum_tbl(&fp->tbl, &child->tbl);
		}
	}
	fp->read_tbl = TRUE;
}

/*
 * zero_tbl:
 *	Zero out the fields we care about in a tbl structure.
 */
zero_tbl(tp)
register STRFILE	*tp;
{
	tp->str_numstr = 0;
	tp->str_longlen = 0;
	tp->str_shortlen = -1;
}

/*
 * sum_tbl:
 *	Merge the tbl data of t2 into t1.
 */
sum_tbl(t1, t2)
register STRFILE	*t1, *t2;
{
	t1->str_numstr += t2->str_numstr;
	if (t1->str_longlen < t2->str_longlen)
		t1->str_longlen = t2->str_longlen;
	if (t1->str_shortlen > t2->str_shortlen)
		t1->str_shortlen = t2->str_shortlen;
}

#define	STR(str)	((str) == NULL ? "NULL" : (str))

/*
 * print_file_list:
 *	Print out the file list
 */
print_file_list()
{
	print_list(File_list, 0);
}

/*
 * print_list:
 *	Print out the actual list, recursively.
 */
print_list(list, lev)
register FILEDESC	*list;
int			lev;
{
	while (list != NULL) {
		fprintf(stderr, "%*s", lev * 4, "");
		if (list->percent == NO_PROB)
			fprintf(stderr, "___%%");
		else
			fprintf(stderr, "%3d%%", list->percent);
		fprintf(stderr, " %s", STR(list->name));
		DPRINTF(1, (stderr, " (%s, %s, %s)\n", STR(list->path),
			    STR(list->datfile), STR(list->posfile)));
		putc('\n', stderr);
		if (list->child != NULL)
			print_list(list->child, lev + 1);
		list = list->next;
	}
}

#ifndef	NO_REGEX
/*
 * conv_pat:
 *	Convert the pattern to an ignore-case equivalent.
 */
char *
conv_pat(orig)
register char	*orig;
{
	register char		*sp;
	register unsigned int	cnt;
	register char		*new;

	cnt = 1;	/* allow for '\0' */
	for (sp = orig; *sp != '\0'; sp++)
		if (isalpha(*sp))
			cnt += 4;
		else
			cnt++;
	if ((new = malloc(cnt)) == NULL) {
		fprintf(stderr, "pattern too long for ignoring case\n");
		exit(1);
	}

	for (sp = new; *orig != '\0'; orig++) {
		if (islower(*orig)) {
			*sp++ = '[';
			*sp++ = *orig;
			*sp++ = toupper(*orig);
			*sp++ = ']';
		}
		else if (isupper(*orig)) {
			*sp++ = '[';
			*sp++ = *orig;
			*sp++ = tolower(*orig);
			*sp++ = ']';
		}
		else
			*sp++ = *orig;
	}
	*sp = '\0';
	return new;
}

/*
 * find_matches:
 *	Find all the fortunes which match the pattern we've been given.
 */
find_matches()
{
	Fort_len = maxlen_in_list(File_list);
	DPRINTF(2, (stderr, "Maximum length is %d\n", Fort_len));
	Fortbuf = do_malloc((unsigned int) Fort_len);

	Found_one = FALSE;
	matches_in_list(File_list);
	return Found_one;
	/* NOTREACHED */
}

/*
 * maxlen_in_list
 *	Return the maximum fortune len in the file list.
 */
maxlen_in_list(list)
FILEDESC	*list;
{
	register FILEDESC	*fp;
	register int		len, maxlen;

	maxlen = 0;
	for (fp = list; fp != NULL; fp = fp->next) {
		if (fp->child != NULL) {
			if ((len = maxlen_in_list(fp->child)) > maxlen)
				maxlen = len;
		}
		else {
			get_tbl(fp);
			if (fp->tbl.str_longlen > maxlen)
				maxlen = fp->tbl.str_longlen;
		}
	}
	return maxlen;
}

/*
 * matches_in_list
 *	Print out the matches from the files in the list.
 */
matches_in_list(list)
FILEDESC	*list;
{
	register char		*sp;
	register FILEDESC	*fp;
	int			in_file;

	for (fp = list; fp != NULL; fp = fp->next) {
		if (fp->child != NULL) {
			matches_in_list(fp->child);
			continue;
		}
		DPRINTF(1, (stderr, "searching in %s\n", fp->path));
		open_fp(fp);
		sp = Fortbuf;
		in_file = FALSE;
		while (fgets(sp, Fort_len, fp->inf) != NULL)
			if (!STR_ENDSTRING(sp, fp->tbl))
				sp += strlen(sp);
			else {
				*sp = '\0';
				if (RE_EXEC(Fortbuf)) {
					printf("%c%c", fp->tbl.str_delim, fp->tbl.str_delim);
					if (!in_file) {
						printf(" (%s)", fp->name);
						Found_one = TRUE;
						in_file = TRUE;
					}
					putchar('\n');
					(void) fwrite(Fortbuf, 1, (sp - Fortbuf), stdout);
				}
				sp = Fortbuf;
			}
	}
}
# endif	/* NO_REGEX */
