// expansion.c - Handle ~ and $VAR expansion
#include "expansion.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

char *
expand_string(const char *s)
{
    if (!s)
        return NULL;

    char *result = malloc(4096);
    char *out = result;
    const char *in = s;

    while (*in && out - result < 4095) {
        // Handle ~
        if (*in == '~') {
            const char *home = getenv("HOME");
            if (!home) {
                struct passwd *pw = getpwuid(getuid());
                home = pw ? pw->pw_dir : "/root";
            }
            strcpy(out, home);
            out += strlen(home);
            in++;
        }
        // Handle $VARIABLE
        else if (*in == '$') {
            in++;
            // Extract variable name
            char varname[256] = {0};
            char *vn = varname;
            while (*in && ((*in >= 'a' && *in <= 'z') ||
                           (*in >= 'A' && *in <= 'Z') ||
                           (*in >= '0' && *in <= '9') ||
                           *in == '_') && vn - varname < 255) {
                *vn++ = *in++;
            }
            *vn = '\0';

            if (strlen(varname) > 0) {
                const char *val = getenv(varname);
                if (val) {
                    strcpy(out, val);
                    out += strlen(val);
                }
            }
        }
        else {
            *out++ = *in++;
        }
    }
    *out = '\0';

    return result;
}
