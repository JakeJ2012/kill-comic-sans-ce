# ----------------------------
# Makefile Options
# ----------------------------

NAME = COMICSNS
DESCRIPTION = "KILL COMIC SANS"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
