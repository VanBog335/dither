/*
Return:
  0 - success
 -1 - NULL-pointer
 -2 - wrong format 
 -3 - overflow/negative value
*/

int parse_size(const char *str, int *out_w, int *out_h) {
    if (!str || !out_w || !out_h) return -1;

    const char *p = str;

    if (!isdigit((uint8_t)*p)) return -2;

    errno = 0;
    char *endptr;

    long w = strtol(p, &endptr, 10);
    if (endptr == p) return -2;
    if (errno == ERANGE || w <= 0 || w > INT_MAX) return -3;

    p = endptr;

    if (*p != 'x' && *p != 'X') return -2;
    ++p;

    if (!isdigit((uint8_t)*p)) return -2;

    errno = 0;
    long h = strtol(p, &endptr, 10);
    if (endptr == p) return -2;
    if (errno == ERANGE || h <= 0 || h > INT_MAX) return -3;

    *out_w = (int)w;
    *out_h = (int)h;
    return 0;
}
