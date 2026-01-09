void ui(void)
{
	int i, j;

	printf(CLEAR);

	printf("\033[1;34m╔");
	for (i = 0; i < MAP_W; i++)
		printf("══");
	printf("╗\033[0m\n");

	for (i = 0; i < MAP_H; i++) {
		printf("\033[1;34m║\033[0m");

		for (j = 0; j < MAP_W; j++) {
			uint8_t t = map[j][i].type;

			switch (t) {
			case HEAD:
				printf("\033[1;33m%s\033[0m", C_HEAD);
				break;
			case BODY:
				printf("\033[32m%s\033[0m", C_BODY);
				break;
			case FRUIT:
				printf("\033[31m%s\033[0m", C_FRUIT);
				break;
			case POSION:
				printf("\033[35m%s\033[0m", C_POSION);
				break;
			default:
				printf("%s", C_AIR);
				break;
			}
		}
		printf("\033[1;34m║\033[0m\n");
	}

	printf("\033[1;34m╚");
	for (i = 0; i < MAP_W; i++)
		printf("══");
	printf("╝\033[0m\n");

	debug();
	printf("Length: %-3d Score: %-3d\n", TAILPOS, TAILPOS - 2);
}
