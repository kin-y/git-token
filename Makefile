# Define MOZILLA_SHA1 environment variable when running make to make use of
# a bundled SHA1 routine coming from Mozilla. It is GPL'd and should be fast
# on non-x86 architectures (e.g. PowerPC), while the OpenSSL version (default
# choice) has very fast version optimized for i586.
#
# Define NO_OPENSSL environment variable if you do not have OpenSSL. You will
# miss out git-rev-list --merge-order. This also implies MOZILLA_SHA1.
#
# Define NO_CURL if you do not have curl installed.  git-http-pull is not
# built, and you cannot use http:// and https:// transports.
#
# Define PPC_SHA1 environment variable when running make to make use of
# a bundled SHA1 routine optimized for PowerPC.
#
# Define NEEDS_SSL_WITH_CRYPTO if you need -lcrypto with -lssl (Darwin).
#
# Define NEEDS_LIBICONV if linking with libc is not enough (Darwin).
#
# Define NEEDS_SOCKET if linking with libc is not enough (SunOS,
# Patrick Mauritz).
#
# Define NO_GETDOMAINNAME if your library lack it (SunOS, Patrick Mauritz).
#
# Define COLLISION_CHECK below if you believe that SHA1's
# 1461501637330902918203684832716283019655932542976 hashes do not give you
# sufficient guarantee that no collisions between objects will ever happen.

# DEFINES += -DCOLLISION_CHECK

# Define USE_NSEC below if you want git to care about sub-second file mtimes
# and ctimes. Note that you need recent glibc (at least 2.2.4) for this, and
# it will BREAK YOUR LOCAL DIFFS! show-diff and anything using it will likely
# randomly break unless your underlying filesystem supports those sub-second
# times (my ext3 doesn't).

# DEFINES += -DUSE_NSEC

# Define USE_STDEV below if you want git to care about the underlying device
# change being considered an inode change from the update-cache perspective.

# DEFINES += -DUSE_STDEV

GIT_VERSION = 0.99.6

CFLAGS = -g -O2 -Wall
ALL_CFLAGS = $(CFLAGS) $(PLATFORM_DEFINES) $(DEFINES)

prefix = $(HOME)
bindir = $(prefix)/bin
template_dir = $(prefix)/share/git-core/templates/
GIT_PYTHON_DIR = $(prefix)/share/git-core/python
# DESTDIR=

CC = gcc
AR = ar
INSTALL = install
RPMBUILD = rpmbuild

# sparse is architecture-neutral, which means that we need to tell it
# explicitly what architecture to check for. Fix this up for yours..
SPARSE_FLAGS = -D__BIG_ENDIAN__ -D__powerpc__



### --- END CONFIGURATION SECTION ---

SCRIPT_SH = \
	git-add.sh git-bisect.sh git-branch.sh git-checkout.sh \
	git-cherry.sh git-clone.sh git-commit.sh \
	git-count-objects.sh git-diff.sh git-fetch.sh \
	git-format-patch.sh git-log.sh git-ls-remote.sh \
	git-merge-one-file.sh git-octopus.sh git-parse-remote.sh \
	git-prune.sh git-pull.sh git-push.sh git-rebase.sh \
	git-repack.sh git-request-pull.sh git-reset.sh \
	git-resolve.sh git-revert.sh git-sh-setup.sh git-status.sh \
	git-tag.sh git-verify-tag.sh git-whatchanged.sh git.sh \
	git-applymbox.sh git-applypatch.sh \
	git-merge.sh git-merge-stupid.sh git-merge-octopus.sh \
	git-merge-resolve.sh git-grep.sh

SCRIPT_PERL = \
	git-archimport.perl git-cvsimport.perl git-relink.perl \
	git-rename.perl git-shortlog.perl

SCRIPT_PYTHON = \
	git-merge-fredrik.py

# The ones that do not have to link with lcrypto nor lz.
SIMPLE_PROGRAMS = \
	git-get-tar-commit-id git-mailinfo git-mailsplit git-stripspace \
	git-daemon git-var

# ... and all the rest
PROGRAMS = \
	git-apply git-build-rev-cache git-cat-file \
	git-checkout-index git-clone-pack git-commit-tree \
	git-convert-objects git-diff-files \
	git-diff-helper git-diff-index git-diff-stages \
	git-diff-tree git-export git-fetch-pack git-fsck-objects \
	git-hash-object git-init-db \
	git-local-fetch git-ls-files git-ls-tree git-merge-base \
	git-merge-index git-mktag git-pack-objects git-patch-id \
	git-peek-remote git-prune-packed git-read-tree \
	git-receive-pack git-rev-list git-rev-parse \
	git-rev-tree git-send-pack git-show-branch \
	git-show-index git-show-rev-cache git-ssh-fetch \
	git-ssh-upload git-tar-tree git-unpack-file \
	git-unpack-objects git-update-index git-update-server-info \
	git-upload-pack git-verify-pack git-write-tree \
	$(SIMPLE_PROGRAMS)

PYMODULES = \
	gitMergeCommon.py

ifdef WITH_SEND_EMAIL
	SCRIPT_PERL += git-send-email.perl
endif

ifndef NO_CURL
	PROGRAMS += git-http-fetch
endif

LIB_FILE=libgit.a

LIB_H = \
	blob.h cache.h commit.h count-delta.h csum-file.h delta.h \
	diff.h epoch.h object.h pack.h pkt-line.h quote.h refs.h \
	rev-cache.h run-command.h strbuf.h tag.h tree.h

DIFF_OBJS = \
	diff.o diffcore-break.o diffcore-order.o diffcore-pathspec.o \
	diffcore-pickaxe.o diffcore-rename.o

LIB_OBJS = \
	blob.o commit.o connect.o count-delta.o csum-file.o \
	date.o diff-delta.o entry.o ident.o index.o \
	object.o pack-check.o patch-delta.o path.o pkt-line.o \
	quote.o read-cache.o refs.o rev-cache.o run-command.o \
	server-info.o setup.o sha1_file.o sha1_name.o strbuf.o \
	tag.o tree.o usage.o $(DIFF_OBJS)

LIBS = $(LIB_FILE)
LIBS += -lz

ifeq ($(shell uname -s),Darwin)
	NEEDS_SSL_WITH_CRYPTO = YesPlease
	NEEDS_LIBICONV = YesPlease
endif
ifeq ($(shell uname -s),SunOS)
	NEEDS_SOCKET = YesPlease
	PLATFORM_DEFINES += -DNO_GETDOMAINNAME=1
endif

ifndef SHELL_PATH
	SHELL_PATH = /bin/sh
endif
ifndef PERL_PATH
	PERL_PATH = /usr/bin/perl
endif
ifndef PYTHON_PATH
	PYTHON_PATH = /usr/bin/python
endif

ifndef NO_OPENSSL
	LIB_OBJS += epoch.o
	OPENSSL_LIBSSL = -lssl
else
	DEFINES += '-DNO_OPENSSL'
	MOZILLA_SHA1 = 1
	OPENSSL_LIBSSL =
endif
ifdef NEEDS_SSL_WITH_CRYPTO
	LIB_4_CRYPTO = -lcrypto -lssl
else
	LIB_4_CRYPTO = -lcrypto
endif
ifdef NEEDS_LIBICONV
	LIB_4_ICONV = -liconv
else
	LIB_4_ICONV =
endif
ifdef MOZILLA_SHA1
	SHA1_HEADER = "mozilla-sha1/sha1.h"
	LIB_OBJS += mozilla-sha1/sha1.o
else
	ifdef PPC_SHA1
		SHA1_HEADER = "ppc/sha1.h"
		LIB_OBJS += ppc/sha1.o ppc/sha1ppc.o
	else
		SHA1_HEADER = <openssl/sha.h>
		LIBS += $(LIB_4_CRYPTO)
	endif
endif
ifdef NEEDS_SOCKET
	LIBS += -lsocket
	SIMPLE_LIB += -lsocket
endif

DEFINES += '-DSHA1_HEADER=$(SHA1_HEADER)'

SCRIPTS = $(patsubst %.sh,%,$(SCRIPT_SH)) \
	  $(patsubst %.perl,%,$(SCRIPT_PERL)) \
	  $(patsubst %.py,%,$(SCRIPT_PYTHON)) \
	  gitk

### Build rules

all: $(PROGRAMS) $(SCRIPTS)

all:
	$(MAKE) -C templates

git: git.sh Makefile
	rm -f $@+ $@
	sed -e 's/@@GIT_VERSION@@/$(GIT_VERSION)/g' <$@.sh >$@+
	chmod +x $@+
	mv $@+ $@

$(filter-out git,$(patsubst %.sh,%,$(SCRIPT_SH))) : % : %.sh
	rm -f $@
	sed -e '1s|#!.*/sh|#!$(SHELL_PATH)|' $@.sh >$@
	chmod +x $@

$(patsubst %.perl,%,$(SCRIPT_PERL)) : % : %.perl
	rm -f $@
	sed -e '1s|#!.*perl|#!$(PERL_PATH)|' $@.perl >$@
	chmod +x $@

$(patsubst %.py,%,$(SCRIPT_PYTHON)) : % : %.py
	rm -f $@
	sed -e '1s|#!.*python|#!$(PYTHON_PATH)|' \
	    -e 's|@@GIT_PYTHON_PATH@@|$(GIT_PYTHON_DIR)|g' \
		$@.py >$@
	chmod +x $@

%.o: %.c
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<
%.o: %.S
	$(CC) -o $*.o -c $(ALL_CFLAGS) $<

git-%: %.o $(LIB_FILE)
	$(CC) $(ALL_CFLAGS) -o $@ $(filter %.o,$^) $(LIBS)

git-mailinfo : SIMPLE_LIB += $(LIB_4_ICONV)
$(SIMPLE_PROGRAMS) : $(LIB_FILE)
$(SIMPLE_PROGRAMS) : git-% : %.o
	$(CC) $(ALL_CFLAGS) -o $@ $(filter %.o,$^) $(LIB_FILE) $(SIMPLE_LIB)

git-http-fetch: fetch.o
git-local-fetch: fetch.o
git-ssh-fetch: rsh.o fetch.o
git-ssh-upload: rsh.o

git-http-fetch: LIBS += -lcurl
git-rev-list: LIBS += $(OPENSSL_LIBSSL)

init-db.o: init-db.c
	$(CC) -c $(ALL_CFLAGS) \
		-DDEFAULT_GIT_TEMPLATE_DIR='"$(template_dir)"' $*.c

$(LIB_OBJS): $(LIB_H)
$(patsubst git-%,%.o,$(PROGRAMS)): $(LIB_H)
$(DIFF_OBJS): diffcore.h

$(LIB_FILE): $(LIB_OBJS)
	$(AR) rcs $@ $(LIB_OBJS)

doc:
	$(MAKE) -C Documentation all


### Testing rules

test: all
	$(MAKE) -C t/ all

test-date: test-date.c date.o
	$(CC) $(ALL_CFLAGS) -o $@ test-date.c date.o

test-delta: test-delta.c diff-delta.o patch-delta.o
	$(CC) $(ALL_CFLAGS) -o $@ $^

check:
	for i in *.c; do sparse $(ALL_CFLAGS) $(SPARSE_FLAGS) $$i; done



### Installation rules

install: $(PROGRAMS) $(SCRIPTS)
	$(INSTALL) -m755 -d $(DESTDIR)$(bindir)
	$(INSTALL) $(PROGRAMS) $(SCRIPTS) $(DESTDIR)$(bindir)
	$(INSTALL) git-revert $(DESTDIR)$(bindir)/git-cherry-pick
	sh ./cmd-rename.sh $(DESTDIR)$(bindir)
	$(MAKE) -C templates install
	$(INSTALL) -m755 -d $(DESTDIR)$(GIT_PYTHON_DIR)
	$(INSTALL) $(PYMODULES) $(DESTDIR)$(GIT_PYTHON_DIR)

install-doc:
	$(MAKE) -C Documentation install




### Maintainer's dist rules

git-core.spec: git-core.spec.in Makefile
	sed -e 's/@@VERSION@@/$(GIT_VERSION)/g' < $< > $@

GIT_TARNAME=git-core-$(GIT_VERSION)
dist: git-core.spec git-tar-tree
	./git-tar-tree HEAD $(GIT_TARNAME) > $(GIT_TARNAME).tar
	@mkdir -p $(GIT_TARNAME)
	@cp git-core.spec $(GIT_TARNAME)
	tar rf $(GIT_TARNAME).tar $(GIT_TARNAME)/git-core.spec
	@rm -rf $(GIT_TARNAME)
	gzip -f -9 $(GIT_TARNAME).tar

rpm: dist
	$(RPMBUILD) -ta git-core-$(GIT_VERSION).tar.gz

deb: dist
	rm -rf $(GIT_TARNAME)
	tar zxf $(GIT_TARNAME).tar.gz
	dpkg-source -b $(GIT_TARNAME)
	cd $(GIT_TARNAME) && fakeroot debian/rules binary

### Cleaning rules

clean:
	rm -f *.o mozilla-sha1/*.o ppc/*.o $(PROGRAMS) $(LIB_FILE)
	rm -f $(filter-out gitk,$(SCRIPTS))
	rm -f git-core.spec
	rm -rf $(GIT_TARNAME)
	rm -f $(GIT_TARNAME).tar.gz git-core_$(GIT_VERSION)-*.tar.gz
	rm -f git-core_$(GIT_VERSION)-*.deb git-core_$(GIT_VERSION)-*.dsc
	rm -f git-tk_$(GIT_VERSION)-*.deb
	$(MAKE) -C Documentation/ clean
	$(MAKE) -C templates/ clean
	$(MAKE) -C t/ clean
