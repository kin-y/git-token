begin_unit
begin_comment
comment|/*  * Another stupid program, this one parsing the headers of an  * email to figure out authorship and subject  */
end_comment
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
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
begin_decl_stmt
DECL|variable|cmitmsg
DECL|variable|patchfile
specifier|static
name|FILE
modifier|*
name|cmitmsg
decl_stmt|,
modifier|*
name|patchfile
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|date
specifier|static
name|char
name|date
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|name
specifier|static
name|char
name|name
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|email
specifier|static
name|char
name|email
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|subject
specifier|static
name|char
name|subject
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|sanity_check
specifier|static
name|char
modifier|*
name|sanity_check
parameter_list|(
name|char
modifier|*
name|name
parameter_list|,
name|char
modifier|*
name|email
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|3
operator|||
name|len
operator|>
literal|60
condition|)
return|return
name|email
return|;
if|if
condition|(
name|strchr
argument_list|(
name|name
argument_list|,
literal|'@'
argument_list|)
operator|||
name|strchr
argument_list|(
name|name
argument_list|,
literal|'<'
argument_list|)
operator|||
name|strchr
argument_list|(
name|name
argument_list|,
literal|'>'
argument_list|)
condition|)
return|return
name|email
return|;
return|return
name|name
return|;
block|}
end_function
begin_function
DECL|function|handle_from
specifier|static
name|int
name|handle_from
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|char
modifier|*
name|at
init|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|'@'
argument_list|)
decl_stmt|;
name|char
modifier|*
name|dst
decl_stmt|;
if|if
condition|(
operator|!
name|at
condition|)
return|return
literal|0
return|;
comment|/* 	 * If we already have one email, don't take any confusing lines 	 */
if|if
condition|(
operator|*
name|email
operator|&&
name|strchr
argument_list|(
name|at
operator|+
literal|1
argument_list|,
literal|'@'
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
name|at
operator|>
name|line
condition|)
block|{
name|char
name|c
init|=
name|at
index|[
operator|-
literal|1
index|]
decl_stmt|;
if|if
condition|(
name|isspace
argument_list|(
name|c
argument_list|)
operator|||
name|c
operator|==
literal|'<'
condition|)
break|break;
name|at
operator|--
expr_stmt|;
block|}
name|dst
operator|=
name|email
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|at
decl_stmt|;
if|if
condition|(
operator|!
name|c
operator|||
name|c
operator|==
literal|'>'
operator|||
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
break|break;
operator|*
name|at
operator|++
operator|=
literal|' '
expr_stmt|;
operator|*
name|dst
operator|++
operator|=
name|c
expr_stmt|;
block|}
operator|*
name|dst
operator|++
operator|=
literal|0
expr_stmt|;
name|at
operator|=
name|line
operator|+
name|strlen
argument_list|(
name|line
argument_list|)
expr_stmt|;
while|while
condition|(
name|at
operator|>
name|line
condition|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
operator|--
name|at
decl_stmt|;
if|if
condition|(
name|isalnum
argument_list|(
name|c
argument_list|)
condition|)
break|break;
operator|*
name|at
operator|=
literal|0
expr_stmt|;
block|}
name|at
operator|=
name|line
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|at
decl_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
break|break;
if|if
condition|(
name|isalnum
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|at
operator|++
expr_stmt|;
block|}
name|at
operator|=
name|sanity_check
argument_list|(
name|at
argument_list|,
name|email
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|name
argument_list|,
name|at
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|handle_date
specifier|static
name|void
name|handle_date
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|strcpy
argument_list|(
name|date
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|handle_subject
specifier|static
name|void
name|handle_subject
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|strcpy
argument_list|(
name|subject
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_subject_line
specifier|static
name|void
name|add_subject_line
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|line
argument_list|)
condition|)
name|line
operator|++
expr_stmt|;
operator|*
operator|--
name|line
operator|=
literal|' '
expr_stmt|;
name|strcat
argument_list|(
name|subject
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_line
specifier|static
name|void
name|check_line
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
specifier|static
name|int
name|cont
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"From:"
argument_list|,
literal|5
argument_list|)
operator|&&
name|isspace
argument_list|(
name|line
index|[
literal|5
index|]
argument_list|)
condition|)
block|{
name|handle_from
argument_list|(
name|line
operator|+
literal|6
argument_list|)
expr_stmt|;
name|cont
operator|=
literal|0
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"Date:"
argument_list|,
literal|5
argument_list|)
operator|&&
name|isspace
argument_list|(
name|line
index|[
literal|5
index|]
argument_list|)
condition|)
block|{
name|handle_date
argument_list|(
name|line
operator|+
literal|6
argument_list|)
expr_stmt|;
name|cont
operator|=
literal|0
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"Subject:"
argument_list|,
literal|8
argument_list|)
operator|&&
name|isspace
argument_list|(
name|line
index|[
literal|8
index|]
argument_list|)
condition|)
block|{
name|handle_subject
argument_list|(
name|line
operator|+
literal|9
argument_list|)
expr_stmt|;
name|cont
operator|=
literal|1
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|isspace
argument_list|(
operator|*
name|line
argument_list|)
condition|)
block|{
switch|switch
condition|(
name|cont
condition|)
block|{
case|case
literal|0
case|:
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"I don't do 'Date:' or 'From:' line continuations\n"
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
name|add_subject_line
argument_list|(
name|line
argument_list|)
expr_stmt|;
return|return;
default|default:
break|break;
block|}
block|}
name|cont
operator|=
operator|-
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cleanup_subject
specifier|static
name|char
modifier|*
name|cleanup_subject
parameter_list|(
name|char
modifier|*
name|subject
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
control|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|int
name|len
decl_stmt|,
name|remove
decl_stmt|;
switch|switch
condition|(
operator|*
name|subject
condition|)
block|{
case|case
literal|'r'
case|:
case|case
literal|'R'
case|:
if|if
condition|(
operator|!
name|memcmp
argument_list|(
literal|"e:"
argument_list|,
name|subject
operator|+
literal|1
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|subject
operator|+=
literal|3
expr_stmt|;
continue|continue;
block|}
break|break;
case|case
literal|' '
case|:
case|case
literal|'\t'
case|:
case|case
literal|':'
case|:
name|subject
operator|++
expr_stmt|;
continue|continue;
case|case
literal|'['
case|:
name|p
operator|=
name|strchr
argument_list|(
name|subject
argument_list|,
literal|']'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
block|{
name|subject
operator|++
expr_stmt|;
continue|continue;
block|}
name|len
operator|=
name|strlen
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|remove
operator|=
name|p
operator|-
name|subject
expr_stmt|;
if|if
condition|(
name|remove
operator|<=
name|len
operator|*
literal|2
condition|)
block|{
name|subject
operator|=
name|p
operator|+
literal|1
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
return|return
name|subject
return|;
block|}
block|}
end_function
begin_function
DECL|function|cleanup_space
specifier|static
name|void
name|cleanup_space
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|)
block|{
name|unsigned
name|char
name|c
decl_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|buf
operator|)
operator|!=
literal|0
condition|)
block|{
name|buf
operator|++
expr_stmt|;
if|if
condition|(
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
block|{
name|buf
index|[
operator|-
literal|1
index|]
operator|=
literal|' '
expr_stmt|;
name|c
operator|=
operator|*
name|buf
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|buf
argument_list|)
decl_stmt|;
name|memmove
argument_list|(
name|buf
argument_list|,
name|buf
operator|+
literal|1
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|c
operator|=
operator|*
name|buf
expr_stmt|;
block|}
block|}
block|}
block|}
end_function
begin_function
DECL|function|handle_rest
specifier|static
name|void
name|handle_rest
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|sub
init|=
name|cleanup_subject
argument_list|(
name|subject
argument_list|)
decl_stmt|;
name|cleanup_space
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|cleanup_space
argument_list|(
name|date
argument_list|)
expr_stmt|;
name|cleanup_space
argument_list|(
name|email
argument_list|)
expr_stmt|;
name|cleanup_space
argument_list|(
name|sub
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Author: %s\nEmail: %s\nSubject: %s\nDate: %s\n\n"
argument_list|,
name|name
argument_list|,
name|email
argument_list|,
name|sub
argument_list|,
name|date
argument_list|)
expr_stmt|;
name|FILE
modifier|*
name|out
init|=
name|cmitmsg
decl_stmt|;
do|do
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
literal|"diff -"
argument_list|,
name|line
argument_list|,
literal|6
argument_list|)
operator|||
operator|!
name|memcmp
argument_list|(
literal|"---"
argument_list|,
name|line
argument_list|,
literal|3
argument_list|)
operator|||
operator|!
name|memcmp
argument_list|(
literal|"Index: "
argument_list|,
name|line
argument_list|,
literal|7
argument_list|)
condition|)
name|out
operator|=
name|patchfile
expr_stmt|;
name|fputs
argument_list|(
name|line
argument_list|,
name|out
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
do|;
if|if
condition|(
name|out
operator|==
name|cmitmsg
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"No patch found\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|cmitmsg
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|patchfile
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|eatspace
specifier|static
name|int
name|eatspace
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
while|while
condition|(
name|len
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
name|line
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|line
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|handle_body
specifier|static
name|void
name|handle_body
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|has_from
init|=
literal|0
decl_stmt|;
comment|/* First line of body can be a From: */
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|len
init|=
name|eatspace
argument_list|(
name|line
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
continue|continue;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
literal|"From:"
argument_list|,
name|line
argument_list|,
literal|5
argument_list|)
operator|&&
name|isspace
argument_list|(
name|line
index|[
literal|5
index|]
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|has_from
operator|&&
name|handle_from
argument_list|(
name|line
operator|+
literal|6
argument_list|)
condition|)
block|{
name|has_from
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
block|}
name|line
index|[
name|len
index|]
operator|=
literal|'\n'
expr_stmt|;
name|handle_rest
argument_list|()
expr_stmt|;
break|break;
block|}
block|}
end_function
begin_function
DECL|function|usage
specifier|static
name|void
name|usage
parameter_list|(
name|void
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"mailinfo msg-file path-file< email\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
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
if|if
condition|(
name|argc
operator|!=
literal|3
condition|)
name|usage
argument_list|()
expr_stmt|;
name|cmitmsg
operator|=
name|fopen
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmitmsg
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|patchfile
operator|=
name|fopen
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|patchfile
condition|)
block|{
name|perror
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|len
init|=
name|eatspace
argument_list|(
name|line
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
block|{
name|handle_body
argument_list|()
expr_stmt|;
break|break;
block|}
name|check_line
argument_list|(
name|line
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
