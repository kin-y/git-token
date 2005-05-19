begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<pwd.h>
end_include
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<ctype.h>
end_include
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_define
DECL|macro|BLOCKING
define|#
directive|define
name|BLOCKING
value|(1ul<< 14)
end_define
begin_comment
comment|/*  * FIXME! Share the code with "write-tree.c"  */
end_comment
begin_function
DECL|function|init_buffer
specifier|static
name|void
name|init_buffer
parameter_list|(
name|char
modifier|*
modifier|*
name|bufp
parameter_list|,
name|unsigned
name|int
modifier|*
name|sizep
parameter_list|)
block|{
name|char
modifier|*
name|buf
init|=
name|xmalloc
argument_list|(
name|BLOCKING
argument_list|)
decl_stmt|;
operator|*
name|sizep
operator|=
literal|0
expr_stmt|;
operator|*
name|bufp
operator|=
name|buf
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_buffer
specifier|static
name|void
name|add_buffer
parameter_list|(
name|char
modifier|*
modifier|*
name|bufp
parameter_list|,
name|unsigned
name|int
modifier|*
name|sizep
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
name|one_line
index|[
literal|2048
index|]
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|int
name|len
decl_stmt|;
name|unsigned
name|long
name|alloc
decl_stmt|,
name|size
decl_stmt|,
name|newsize
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|len
operator|=
name|vsnprintf
argument_list|(
name|one_line
argument_list|,
sizeof|sizeof
argument_list|(
name|one_line
argument_list|)
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
name|size
operator|=
operator|*
name|sizep
expr_stmt|;
name|newsize
operator|=
name|size
operator|+
name|len
expr_stmt|;
name|alloc
operator|=
operator|(
name|size
operator|+
literal|32767
operator|)
operator|&
operator|~
literal|32767
expr_stmt|;
name|buf
operator|=
operator|*
name|bufp
expr_stmt|;
if|if
condition|(
name|newsize
operator|>
name|alloc
condition|)
block|{
name|alloc
operator|=
operator|(
name|newsize
operator|+
literal|32767
operator|)
operator|&
operator|~
literal|32767
expr_stmt|;
name|buf
operator|=
name|xrealloc
argument_list|(
name|buf
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
operator|*
name|bufp
operator|=
name|buf
expr_stmt|;
block|}
operator|*
name|sizep
operator|=
name|newsize
expr_stmt|;
name|memcpy
argument_list|(
name|buf
operator|+
name|size
argument_list|,
name|one_line
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_special
specifier|static
name|void
name|remove_special
parameter_list|(
name|char
modifier|*
name|p
parameter_list|)
block|{
name|char
name|c
decl_stmt|;
name|char
modifier|*
name|dst
init|=
name|p
decl_stmt|,
modifier|*
name|src
init|=
name|p
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|c
operator|=
operator|*
name|src
expr_stmt|;
name|src
operator|++
expr_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|'\n'
case|:
case|case
literal|'<'
case|:
case|case
literal|'>'
case|:
continue|continue;
block|}
operator|*
name|dst
operator|++
operator|=
name|c
expr_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
break|break;
block|}
comment|/* 	 * Go back, and remove crud from the end: some people 	 * have commas etc in their gecos field 	 */
name|dst
operator|--
expr_stmt|;
while|while
condition|(
operator|--
name|dst
operator|>=
name|p
condition|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|dst
decl_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|','
case|:
case|case
literal|';'
case|:
case|case
literal|'.'
case|:
operator|*
name|dst
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
block|}
end_function
begin_function
DECL|function|check_valid
specifier|static
name|void
name|check_valid
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|expect
parameter_list|)
block|{
name|void
modifier|*
name|buf
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
name|expect
argument_list|)
condition|)
name|die
argument_list|(
literal|"%s is not a valid '%s' object"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|expect
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Having more than two parents is not strange at all, and this is  * how multi-way merges are represented.  */
end_comment
begin_define
DECL|macro|MAXPARENT
define|#
directive|define
name|MAXPARENT
value|(16)
end_define
begin_decl_stmt
DECL|variable|commit_tree_usage
specifier|static
name|char
modifier|*
name|commit_tree_usage
init|=
literal|"git-commit-tree<sha1> [-p<sha1>]*< changelog"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|len
decl_stmt|;
name|int
name|parents
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|parent_sha1
index|[
name|MAXPARENT
index|]
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|commit_sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|gecos
decl_stmt|,
modifier|*
name|realgecos
decl_stmt|,
modifier|*
name|commitgecos
decl_stmt|;
name|char
modifier|*
name|email
decl_stmt|,
modifier|*
name|commitemail
decl_stmt|,
name|realemail
index|[
literal|1000
index|]
decl_stmt|;
name|char
name|date
index|[
literal|20
index|]
decl_stmt|,
name|realdate
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|audate
decl_stmt|;
name|char
name|comment
index|[
literal|1000
index|]
decl_stmt|;
name|struct
name|passwd
modifier|*
name|pw
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|int
name|size
decl_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
operator|||
name|get_sha1_hex
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|tree_sha1
argument_list|)
operator|<
literal|0
condition|)
name|usage
argument_list|(
name|commit_tree_usage
argument_list|)
expr_stmt|;
name|check_valid
argument_list|(
name|tree_sha1
argument_list|,
literal|"tree"
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|2
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|+=
literal|2
control|)
block|{
name|char
modifier|*
name|a
decl_stmt|,
modifier|*
name|b
decl_stmt|;
name|a
operator|=
name|argv
index|[
name|i
index|]
expr_stmt|;
name|b
operator|=
name|argv
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|b
operator|||
name|strcmp
argument_list|(
name|a
argument_list|,
literal|"-p"
argument_list|)
operator|||
name|get_sha1
argument_list|(
name|b
argument_list|,
name|parent_sha1
index|[
name|parents
index|]
argument_list|)
condition|)
name|usage
argument_list|(
name|commit_tree_usage
argument_list|)
expr_stmt|;
name|check_valid
argument_list|(
name|parent_sha1
index|[
name|parents
index|]
argument_list|,
literal|"commit"
argument_list|)
expr_stmt|;
name|parents
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|parents
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Committing initial tree %s\n"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|pw
operator|=
name|getpwuid
argument_list|(
name|getuid
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pw
condition|)
name|die
argument_list|(
literal|"You don't exist. Go away!"
argument_list|)
expr_stmt|;
name|realgecos
operator|=
name|pw
operator|->
name|pw_gecos
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|pw
operator|->
name|pw_name
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|realemail
argument_list|,
name|pw
operator|->
name|pw_name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|realemail
index|[
name|len
index|]
operator|=
literal|'@'
expr_stmt|;
name|gethostname
argument_list|(
name|realemail
operator|+
name|len
operator|+
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
name|realemail
argument_list|)
operator|-
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strchr
argument_list|(
name|realemail
operator|+
name|len
operator|+
literal|1
argument_list|,
literal|'.'
argument_list|)
condition|)
block|{
name|strcat
argument_list|(
name|realemail
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
name|getdomainname
argument_list|(
name|realemail
operator|+
name|strlen
argument_list|(
name|realemail
argument_list|)
argument_list|,
sizeof|sizeof
argument_list|(
name|realemail
argument_list|)
operator|-
name|strlen
argument_list|(
name|realemail
argument_list|)
operator|-
literal|1
argument_list|)
expr_stmt|;
block|}
name|datestamp
argument_list|(
name|realdate
argument_list|,
sizeof|sizeof
argument_list|(
name|realdate
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|date
argument_list|,
name|realdate
argument_list|)
expr_stmt|;
name|commitgecos
operator|=
name|gitenv
argument_list|(
literal|"GIT_COMMITTER_NAME"
argument_list|)
condition|?
else|:
name|realgecos
expr_stmt|;
name|commitemail
operator|=
name|gitenv
argument_list|(
literal|"GIT_COMMITTER_EMAIL"
argument_list|)
condition|?
else|:
name|realemail
expr_stmt|;
name|gecos
operator|=
name|gitenv
argument_list|(
literal|"GIT_AUTHOR_NAME"
argument_list|)
condition|?
else|:
name|realgecos
expr_stmt|;
name|email
operator|=
name|gitenv
argument_list|(
literal|"GIT_AUTHOR_EMAIL"
argument_list|)
condition|?
else|:
name|realemail
expr_stmt|;
name|audate
operator|=
name|gitenv
argument_list|(
literal|"GIT_AUTHOR_DATE"
argument_list|)
expr_stmt|;
if|if
condition|(
name|audate
condition|)
name|parse_date
argument_list|(
name|audate
argument_list|,
name|date
argument_list|,
sizeof|sizeof
argument_list|(
name|date
argument_list|)
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|gecos
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|realgecos
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|commitgecos
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|email
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|realemail
argument_list|)
expr_stmt|;
name|remove_special
argument_list|(
name|commitemail
argument_list|)
expr_stmt|;
name|init_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
name|add_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
literal|"tree %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|tree_sha1
argument_list|)
argument_list|)
expr_stmt|;
comment|/* 	 * NOTE! This ordering means that the same exact tree merged with a 	 * different order of parents will be a _different_ changeset even 	 * if everything else stays the same. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|parents
condition|;
name|i
operator|++
control|)
name|add_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
literal|"parent %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parent_sha1
index|[
name|i
index|]
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Person/date information */
name|add_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
literal|"author %s<%s> %s\n"
argument_list|,
name|gecos
argument_list|,
name|email
argument_list|,
name|date
argument_list|)
expr_stmt|;
name|add_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
literal|"committer %s<%s> %s\n\n"
argument_list|,
name|commitgecos
argument_list|,
name|commitemail
argument_list|,
name|realdate
argument_list|)
expr_stmt|;
comment|/* And add the comment */
while|while
condition|(
name|fgets
argument_list|(
name|comment
argument_list|,
sizeof|sizeof
argument_list|(
name|comment
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
name|add_buffer
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
literal|"%s"
argument_list|,
name|comment
argument_list|)
expr_stmt|;
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
literal|"commit"
argument_list|,
name|commit_sha1
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit_sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
