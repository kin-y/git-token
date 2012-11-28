#!/bin/sh
#
# Copyright (c) 2010 Sverre Rabbelier
#

test_description='Test remote-helper import and export commands'

. ./test-lib.sh

if ! type "${BASH-bash}" >/dev/null 2>&1; then
	skip_all='skipping remote-testgit tests, bash not available'
	test_done
fi

compare_refs() {
	git --git-dir="$1/.git" rev-parse --verify $2 >expect &&
	git --git-dir="$3/.git" rev-parse --verify $4 >actual &&
	test_cmp expect actual
}

test_expect_success 'setup repository' '
	git init --bare server/.git &&
	git clone server public &&
	(cd public &&
	 echo content >file &&
	 git add file &&
	 git commit -m one &&
	 git push origin master)
'

test_expect_success 'cloning from local repo' '
	git clone "testgit::${PWD}/server" localclone &&
	test_cmp public/file localclone/file
'

test_expect_success 'create new commit on remote' '
	(cd public &&
	 echo content >>file &&
	 git commit -a -m two &&
	 git push)
'

test_expect_success 'pulling from local repo' '
	(cd localclone && git pull) &&
	test_cmp public/file localclone/file
'

test_expect_success 'pushing to local repo' '
	(cd localclone &&
	echo content >>file &&
	git commit -a -m three &&
	git push) &&
	compare_refs localclone HEAD server HEAD
'

# Generally, skip this test.  It demonstrates a now-fixed race in
# git-remote-testgit, but is too slow to leave in for general use.
: test_expect_success 'racily pushing to local repo' '
	test_when_finished "rm -rf server2 localclone2" &&
	cp -R server server2 &&
	git clone "testgit::${PWD}/server2" localclone2 &&
	(cd localclone2 &&
	echo content >>file &&
	git commit -a -m three &&
	GIT_REMOTE_TESTGIT_SLEEPY=2 git push) &&
	compare_refs localclone2 HEAD server2 HEAD
'

test_expect_success 'fetch new branch' '
	(cd public &&
	 git checkout -b new &&
	 echo content >>file &&
	 git commit -a -m five &&
	 git push origin new
	) &&
	(cd localclone &&
	 git fetch origin new
	) &&
	compare_refs public HEAD localclone FETCH_HEAD
'

#
# This is only needed because of a bug not detected by this script. It will be
# fixed shortly, but for now lets not cause regressions.
#
test_expect_success 'bump commit in public' '
	(cd public &&
	git checkout master &&
	git pull &&
	echo content >>file &&
	git commit -a -m four &&
	git push) &&
	compare_refs public HEAD server HEAD
'

test_expect_success 'fetch multiple branches' '
	(cd localclone &&
	 git fetch
	) &&
	compare_refs server master localclone refs/remotes/origin/master &&
	compare_refs server new localclone refs/remotes/origin/new
'

test_expect_success 'push when remote has extra refs' '
	(cd localclone &&
	 git reset --hard origin/master &&
	 echo content >>file &&
	 git commit -a -m six &&
	 git push
	) &&
	compare_refs localclone master server master
'

test_expect_success 'push new branch by name' '
	(cd localclone &&
	 git checkout -b new-name  &&
	 echo content >>file &&
	 git commit -a -m seven &&
	 git push origin new-name
	) &&
	compare_refs localclone HEAD server refs/heads/new-name
'

test_expect_failure 'push new branch with old:new refspec' '
	(cd localclone &&
	 git push origin new-name:new-refspec
	) &&
	compare_refs localclone HEAD server refs/heads/new-refspec
'

test_done
