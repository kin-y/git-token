#!/bin/sh
#
# Copyright (c) 2007 Johannes E. Schindelin
#

test_description='git-status'

. ./test-lib.sh

test_expect_success 'setup' '
	: > tracked &&
	: > modified &&
	mkdir dir1 &&
	: > dir1/tracked &&
	: > dir1/modified &&
	mkdir dir2 &&
	: > dir1/tracked &&
	: > dir1/modified &&
	git add . &&

	git status >output &&

	test_tick &&
	git commit -m initial &&
	: > untracked &&
	: > dir1/untracked &&
	: > dir2/untracked &&
	echo 1 > dir1/modified &&
	echo 2 > dir2/modified &&
	echo 3 > dir2/added &&
	git add dir2/added
'

test_expect_success 'status (1)' '

	grep -e "use \"git rm --cached <file>\.\.\.\" to unstage" output

'

cat > expect << \EOF
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#	new file:   dir2/added
#
# Changed but not updated:
#   (use "git add <file>..." to update what will be committed)
#
#	modified:   dir1/modified
#
# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#
#	dir1/untracked
#	dir2/modified
#	dir2/untracked
#	expect
#	output
#	untracked
EOF

test_expect_success 'status (2)' '

	git status > output &&
	git diff expect output

'

cat > expect << \EOF
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#	new file:   ../dir2/added
#
# Changed but not updated:
#   (use "git add <file>..." to update what will be committed)
#
#	modified:   modified
#
# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#
#	untracked
#	../dir2/modified
#	../dir2/untracked
#	../expect
#	../output
#	../untracked
EOF

test_expect_success 'status with relative paths' '

	(cd dir1 && git status) > output &&
	git diff expect output

'

cat > expect << \EOF
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#	new file:   dir2/added
#
# Changed but not updated:
#   (use "git add <file>..." to update what will be committed)
#
#	modified:   dir1/modified
#
# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#
#	dir1/untracked
#	dir2/modified
#	dir2/untracked
#	expect
#	output
#	untracked
EOF

test_expect_success 'status without relative paths' '

	git config status.relativePaths false
	(cd dir1 && git status) > output &&
	git diff expect output

'

cat <<EOF >expect
# On branch master
# Changes to be committed:
#   (use "git reset HEAD <file>..." to unstage)
#
#	modified:   dir1/modified
#
# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#
#	dir1/untracked
#	dir2/
#	expect
#	output
#	untracked
EOF
test_expect_success 'status of partial commit excluding new file in index' '
	git status dir1/modified >output &&
	diff -u expect output
'

test_done
