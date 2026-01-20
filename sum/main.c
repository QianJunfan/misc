#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define PATH_MAX_LEN	1024
#define LINE_MAX_LEN	256

/* scan and print projects matching the target status */
void pr_status(const char *target)
{
	DIR *dr = opendir(".");
	struct dirent *de;
	struct stat st;
	char path[PATH_MAX_LEN];
	char desc[LINE_MAX_LEN];
	char stat_buf[LINE_MAX_LEN];
	FILE *fp;

	if (!dr)
		return;

	printf("%s\n", target);

	while ((de = readdir(dr))) {
		if (de->d_name[0] == '.')
			continue;

		if (stat(de->d_name, &st) == -1 || !S_ISDIR(st.st_mode))
			continue;

		snprintf(path, sizeof(path), "%s/info", de->d_name);
		fp = fopen(path, "r");
		if (!fp)
			continue;

		if (fgets(desc, sizeof(desc), fp) &&
		    fgets(stat_buf, sizeof(stat_buf), fp)) {
			desc[strcspn(desc, "\r\n")] = 0;
			stat_buf[strcspn(stat_buf, "\r\n")] = 0;

			if (!strcmp(stat_buf, target))
				printf("%s\t- %s\n", de->d_name, desc);
		}
		fclose(fp);
	}
	printf("\n");
	closedir(dr);
}

int main(void)
{
	pr_status("[DONE]");
	pr_status("[DOING]");
	pr_status("[ABANDON]");

	return 0;
}
