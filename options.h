#ifndef options_h
#define options_h

extern short opt_port;
extern char const * opt_ip;
extern int opt_sock;
extern int opt_tos;
extern int opt_verbose;
extern int opt_help;

extern void parse_options(int argc, char * const argv[]);

#endif /* options_h */
