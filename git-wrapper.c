#define _BSD_SOURCE 1

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef GIT_PATH
#define GIT_PATH "/usr/lib/git-core/"
#endif

/* This script should be setuid root in order to call setgroups.  We need to do
 * so in order to eliminate any groups that the user may have inherited from the
 * HTTP server.
 */
int main(int argc, char **argv)
{
	uid_t uid;
	gid_t gid, supp;
	struct passwd *pwd;
	int ngroups;

	uid = getuid();

	if (!uid)
		return 1;

	/* Find the user's default group. */
	pwd = getpwuid(uid);
	gid = pwd->pw_gid;

	if (!gid)
		return 1;

	/* Drop privileges. */
	if (setgid(gid))
		return 2;
	if (setgroups(1, &gid))
		return 2;
	if (setuid(uid))
		return 2;
	/* We shouldn't have any more privileges from here on. */
	if (getgid() != gid || getegid() != gid)
		return 3;
	if (getuid() != uid || geteuid() != uid)
		return 3;
	ngroups = getgroups(1, &supp);
	if (ngroups < 0 || ngroups > 1 || (ngroups == 1 && supp != gid))
		return 3;
	execv(GIT_PATH "git-http-backend", argv);
}
