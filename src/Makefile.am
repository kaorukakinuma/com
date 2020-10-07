AUTOMAKE_OPTIONS = subdir-objects

# Name of libraries
lib_LIBRARIES = \
    libipcom.a

# Compile flag for path to include header
AM_CFLAGS = \
    -I$(top_srcdir)/include/impl/mqueue \
    -I$(top_srcdir)/include/impl/socket \
    -I$(top_srcdir)/include

# Source codes included in library
libipcom_a_SOURCES = \
    @srcdir@/impl/mqueue/mqueue_ipcom_client.c \
    @srcdir@/impl/mqueue/mqueue_ipcom_server.c \
    @srcdir@/impl/socket/socket_ipcom_client.c \
    @srcdir@/impl/socket/socket_ipcom_server.c
