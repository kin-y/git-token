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
begin_define
DECL|macro|ORIG_OFFSET
define|#
directive|define
name|ORIG_OFFSET
value|(40)
end_define
begin_comment
comment|/*  * Leave space at the beginning to insert the tag  * once we know how big things are.  *  * FIXME! Share the code with "write-tree.c"  */
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
name|malloc
argument_list|(
name|BLOCKING
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|buf
argument_list|,
literal|0
argument_list|,
name|ORIG_OFFSET
argument_list|)
expr_stmt|;
operator|*
name|sizep
operator|=
name|ORIG_OFFSET
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
name|realloc
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
DECL|function|prepend_integer
specifier|static
name|int
name|prepend_integer
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|val
parameter_list|,
name|int
name|i
parameter_list|)
block|{
name|buffer
index|[
operator|--
name|i
index|]
operator|=
literal|'\0'
expr_stmt|;
do|do
block|{
name|buffer
index|[
operator|--
name|i
index|]
operator|=
literal|'0'
operator|+
operator|(
name|val
operator|%
literal|10
operator|)
expr_stmt|;
name|val
operator|/=
literal|10
expr_stmt|;
block|}
do|while
condition|(
name|val
condition|)
do|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|finish_buffer
specifier|static
name|void
name|finish_buffer
parameter_list|(
name|char
modifier|*
name|tag
parameter_list|,
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
name|int
name|taglen
decl_stmt|;
name|int
name|offset
decl_stmt|;
name|char
modifier|*
name|buf
init|=
operator|*
name|bufp
decl_stmt|;
name|unsigned
name|int
name|size
init|=
operator|*
name|sizep
decl_stmt|;
name|offset
operator|=
name|prepend_integer
argument_list|(
name|buf
argument_list|,
name|size
operator|-
name|ORIG_OFFSET
argument_list|,
name|ORIG_OFFSET
argument_list|)
expr_stmt|;
name|taglen
operator|=
name|strlen
argument_list|(
name|tag
argument_list|)
expr_stmt|;
name|offset
operator|-=
name|taglen
expr_stmt|;
name|buf
operator|+=
name|offset
expr_stmt|;
name|size
operator|-=
name|offset
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|tag
argument_list|,
name|taglen
argument_list|)
expr_stmt|;
operator|*
name|bufp
operator|=
name|buf
expr_stmt|;
operator|*
name|sizep
operator|=
name|size
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
begin_decl_stmt
DECL|variable|month_names
specifier|static
specifier|const
name|char
modifier|*
name|month_names
index|[]
init|=
block|{
literal|"Jan"
block|,
literal|"Feb"
block|,
literal|"Mar"
block|,
literal|"Apr"
block|,
literal|"May"
block|,
literal|"Jun"
block|,
literal|"Jul"
block|,
literal|"Aug"
block|,
literal|"Sep"
block|,
literal|"Oct"
block|,
literal|"Nov"
block|,
literal|"Dec"
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|weekday_names
specifier|static
specifier|const
name|char
modifier|*
name|weekday_names
index|[]
init|=
block|{
literal|"Sun"
block|,
literal|"Mon"
block|,
literal|"Tue"
block|,
literal|"Wed"
block|,
literal|"Thu"
block|,
literal|"Fri"
block|,
literal|"Sat"
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|skipfws
specifier|static
name|char
modifier|*
name|skipfws
parameter_list|(
name|char
modifier|*
name|str
parameter_list|)
block|{
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|str
argument_list|)
condition|)
name|str
operator|++
expr_stmt|;
return|return
name|str
return|;
block|}
end_function
begin_comment
comment|/* Gr. strptime is crap for this; it doesn't have a way to require RFC2822    (i.e. English) day/month names, and it doesn't work correctly with %z. */
end_comment
begin_function
DECL|function|parse_rfc2822_date
specifier|static
name|void
name|parse_rfc2822_date
parameter_list|(
name|char
modifier|*
name|date
parameter_list|,
name|char
modifier|*
name|result
parameter_list|,
name|int
name|maxlen
parameter_list|)
block|{
name|struct
name|tm
name|tm
decl_stmt|;
name|char
modifier|*
name|p
decl_stmt|;
name|int
name|i
decl_stmt|,
name|offset
decl_stmt|;
name|time_t
name|then
decl_stmt|;
name|memset
argument_list|(
operator|&
name|tm
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|tm
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Skip day-name */
name|p
operator|=
name|skipfws
argument_list|(
name|date
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|isdigit
argument_list|(
operator|*
name|p
argument_list|)
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|7
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|p
argument_list|,
name|weekday_names
index|[
name|i
index|]
argument_list|,
literal|3
argument_list|)
operator|&&
name|p
index|[
literal|3
index|]
operator|==
literal|','
condition|)
block|{
name|p
operator|=
name|skipfws
argument_list|(
name|p
operator|+
literal|4
argument_list|)
expr_stmt|;
goto|goto
name|day
goto|;
block|}
block|}
return|return;
block|}
comment|/* day */
name|day
label|:
name|tm
operator|.
name|tm_mday
operator|=
name|strtoul
argument_list|(
name|p
argument_list|,
operator|&
name|p
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|tm
operator|.
name|tm_mday
operator|<
literal|1
operator|||
name|tm
operator|.
name|tm_mday
operator|>
literal|31
condition|)
return|return;
if|if
condition|(
operator|!
name|isspace
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
name|p
operator|=
name|skipfws
argument_list|(
name|p
argument_list|)
expr_stmt|;
comment|/* month */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|12
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|p
argument_list|,
name|month_names
index|[
name|i
index|]
argument_list|,
literal|3
argument_list|)
operator|&&
name|isspace
argument_list|(
name|p
index|[
literal|3
index|]
argument_list|)
condition|)
block|{
name|tm
operator|.
name|tm_mon
operator|=
name|i
expr_stmt|;
name|p
operator|=
name|skipfws
argument_list|(
name|p
operator|+
name|strlen
argument_list|(
name|month_names
index|[
name|i
index|]
argument_list|)
argument_list|)
expr_stmt|;
goto|goto
name|year
goto|;
block|}
block|}
return|return;
comment|/* Error -- bad month */
comment|/* year */
name|year
label|:
name|tm
operator|.
name|tm_year
operator|=
name|strtoul
argument_list|(
name|p
argument_list|,
operator|&
name|p
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tm
operator|.
name|tm_year
operator|&&
operator|!
name|isspace
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
if|if
condition|(
name|tm
operator|.
name|tm_year
operator|>
literal|1900
condition|)
name|tm
operator|.
name|tm_year
operator|-=
literal|1900
expr_stmt|;
name|p
operator|=
name|skipfws
argument_list|(
name|p
argument_list|)
expr_stmt|;
comment|/* hour */
if|if
condition|(
operator|!
name|isdigit
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
name|tm
operator|.
name|tm_hour
operator|=
name|strtoul
argument_list|(
name|p
argument_list|,
operator|&
name|p
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tm
operator|.
name|tm_hour
operator|>
literal|23
condition|)
return|return;
if|if
condition|(
operator|*
name|p
operator|!=
literal|':'
condition|)
return|return;
comment|/* Error -- bad time */
name|p
operator|++
expr_stmt|;
comment|/* minute */
if|if
condition|(
operator|!
name|isdigit
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
name|tm
operator|.
name|tm_min
operator|=
name|strtoul
argument_list|(
name|p
argument_list|,
operator|&
name|p
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tm
operator|.
name|tm_min
operator|>
literal|59
condition|)
return|return;
if|if
condition|(
name|isspace
argument_list|(
operator|*
name|p
argument_list|)
condition|)
goto|goto
name|zone
goto|;
if|if
condition|(
operator|*
name|p
operator|!=
literal|':'
condition|)
return|return;
comment|/* Error -- bad time */
name|p
operator|++
expr_stmt|;
comment|/* second */
if|if
condition|(
operator|!
name|isdigit
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
name|tm
operator|.
name|tm_sec
operator|=
name|strtoul
argument_list|(
name|p
argument_list|,
operator|&
name|p
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tm
operator|.
name|tm_sec
operator|>
literal|59
condition|)
return|return;
if|if
condition|(
operator|!
name|isspace
argument_list|(
operator|*
name|p
argument_list|)
condition|)
return|return;
name|zone
label|:
name|p
operator|=
name|skipfws
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|p
operator|==
literal|'-'
condition|)
name|offset
operator|=
operator|-
literal|60
expr_stmt|;
elseif|else
if|if
condition|(
operator|*
name|p
operator|==
literal|'+'
condition|)
name|offset
operator|=
literal|60
expr_stmt|;
else|else
return|return;
if|if
condition|(
operator|!
name|isdigit
argument_list|(
name|p
index|[
literal|1
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|p
index|[
literal|2
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|p
index|[
literal|3
index|]
argument_list|)
operator|||
operator|!
name|isdigit
argument_list|(
name|p
index|[
literal|4
index|]
argument_list|)
condition|)
return|return;
name|i
operator|=
name|strtoul
argument_list|(
name|p
operator|+
literal|1
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|offset
operator|*=
operator|(
operator|(
name|i
operator|%
literal|100
operator|)
operator|+
operator|(
operator|(
name|i
operator|/
literal|100
operator|)
operator|*
literal|60
operator|)
operator|)
expr_stmt|;
if|if
condition|(
operator|*
operator|(
name|skipfws
argument_list|(
name|p
operator|+
literal|5
argument_list|)
operator|)
condition|)
return|return;
name|then
operator|=
name|mktime
argument_list|(
operator|&
name|tm
argument_list|)
expr_stmt|;
comment|/* mktime appears to ignore the GMT offset, stupidly */
if|if
condition|(
name|then
operator|==
operator|-
literal|1
condition|)
return|return;
name|then
operator|-=
name|offset
expr_stmt|;
name|snprintf
argument_list|(
name|result
argument_list|,
name|maxlen
argument_list|,
literal|"%lu %5.5s"
argument_list|,
name|then
argument_list|,
name|p
argument_list|)
expr_stmt|;
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
comment|/*  * Having more than two parents may be strange, but hey, there's  * no conceptual reason why the file format couldn't accept multi-way  * merges. It might be the "union" of several packages, for example.  *  * I don't really expect that to happen, but this is here to make  * it clear that _conceptually_ it's ok..  */
end_comment
begin_define
DECL|macro|MAXPARENT
define|#
directive|define
name|MAXPARENT
value|(16)
end_define
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
name|time_t
name|now
decl_stmt|;
name|struct
name|tm
modifier|*
name|tm
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
literal|"commit-tree<sha1> [-p<sha1>]*< changelog"
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
name|get_sha1_hex
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
literal|"commit-tree<sha1> [-p<sha1>]*< changelog"
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
name|time
argument_list|(
operator|&
name|now
argument_list|)
expr_stmt|;
name|tm
operator|=
name|localtime
argument_list|(
operator|&
name|now
argument_list|)
expr_stmt|;
name|strftime
argument_list|(
name|realdate
argument_list|,
sizeof|sizeof
argument_list|(
name|realdate
argument_list|)
argument_list|,
literal|"%s %z"
argument_list|,
name|tm
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
name|getenv
argument_list|(
literal|"COMMIT_AUTHOR_NAME"
argument_list|)
condition|?
else|:
name|realgecos
expr_stmt|;
name|commitemail
operator|=
name|getenv
argument_list|(
literal|"COMMIT_AUTHOR_EMAIL"
argument_list|)
condition|?
else|:
name|realemail
expr_stmt|;
name|gecos
operator|=
name|getenv
argument_list|(
literal|"AUTHOR_NAME"
argument_list|)
condition|?
else|:
name|realgecos
expr_stmt|;
name|email
operator|=
name|getenv
argument_list|(
literal|"AUTHOR_EMAIL"
argument_list|)
condition|?
else|:
name|realemail
expr_stmt|;
name|audate
operator|=
name|getenv
argument_list|(
literal|"AUTHOR_DATE"
argument_list|)
expr_stmt|;
if|if
condition|(
name|audate
condition|)
name|parse_rfc2822_date
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
name|finish_buffer
argument_list|(
literal|"commit "
argument_list|,
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|size
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
