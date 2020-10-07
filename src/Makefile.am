AUTOMAKE_OPTIONS = subdir-objects

# Name of libraries
lib_LIBRARIES = \
    libcom.a

# Compile flag for path to include header
AM_CFLAGS = \
    -I$(top_srcdir)/include/impl/mqueue \
    -I$(top_srcdir)/include/impl/socket \
    -I$(top_srcdir)/include

# Source codes included in library
libcom_a_SOURCES = \
    @srcdir@/impl/mqueue/mqueue_com_client.c \
    @srcdir@/impl/mqueue/mqueue_com_server.c \
    @srcdir@/impl/socket/socket_com_client.c \
    @srcdir@/impl/socket/socket_com_server.c
