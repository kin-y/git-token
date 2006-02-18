begin_unit
begin_comment
comment|/*  * rev-parse.c  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_define
DECL|macro|DO_REVS
define|#
directive|define
name|DO_REVS
value|1
end_define
begin_define
DECL|macro|DO_NOREV
define|#
directive|define
name|DO_NOREV
value|2
end_define
begin_define
DECL|macro|DO_FLAGS
define|#
directive|define
name|DO_FLAGS
value|4
end_define
begin_define
DECL|macro|DO_NONFLAGS
define|#
directive|define
name|DO_NONFLAGS
value|8
end_define
begin_decl_stmt
DECL|variable|filter
specifier|static
name|int
name|filter
init|=
operator|~
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|def
specifier|static
name|char
modifier|*
name|def
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|NORMAL
define|#
directive|define
name|NORMAL
value|0
end_define
begin_define
DECL|macro|REVERSED
define|#
directive|define
name|REVERSED
value|1
end_define
begin_decl_stmt
DECL|variable|show_type
specifier|static
name|int
name|show_type
init|=
name|NORMAL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|symbolic
specifier|static
name|int
name|symbolic
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|abbrev
specifier|static
name|int
name|abbrev
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|output_sq
specifier|static
name|int
name|output_sq
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|revs_count
specifier|static
name|int
name|revs_count
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * Some arguments are relevant "revision" arguments,  * others are about output format or other details.  * This sorts it all out.  */
end_comment
begin_function
DECL|function|is_rev_argument
specifier|static
name|int
name|is_rev_argument
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
specifier|static
specifier|const
name|char
modifier|*
name|rev_args
index|[]
init|=
block|{
literal|"--all"
block|,
literal|"--bisect"
block|,
literal|"--dense"
block|,
literal|"--header"
block|,
literal|"--max-age="
block|,
literal|"--max-count="
block|,
literal|"--merge-order"
block|,
literal|"--min-age="
block|,
literal|"--no-merges"
block|,
literal|"--objects"
block|,
literal|"--parents"
block|,
literal|"--pretty"
block|,
literal|"--show-breaks"
block|,
literal|"--sparse"
block|,
literal|"--topo-order"
block|,
literal|"--date-order"
block|,
literal|"--unpacked"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
init|=
name|rev_args
decl_stmt|;
comment|/* accept -<digit>, like traditional "head" */
if|if
condition|(
operator|(
operator|*
name|arg
operator|==
literal|'-'
operator|)
operator|&&
name|isdigit
argument_list|(
name|arg
index|[
literal|1
index|]
argument_list|)
condition|)
return|return
literal|1
return|;
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|str
init|=
operator|*
name|p
operator|++
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|str
condition|)
return|return
literal|0
return|;
name|len
operator|=
name|strlen
argument_list|(
name|str
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
name|str
argument_list|)
operator|||
operator|(
name|str
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'='
operator|&&
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
name|str
argument_list|,
name|len
argument_list|)
operator|)
condition|)
return|return
literal|1
return|;
block|}
block|}
end_function
begin_comment
comment|/* Output argument as a string, either SQ or normal */
end_comment
begin_function
DECL|function|show
specifier|static
name|void
name|show
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
name|output_sq
condition|)
block|{
name|int
name|sq
init|=
literal|'\''
decl_stmt|,
name|ch
decl_stmt|;
name|putchar
argument_list|(
name|sq
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|arg
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|ch
operator|==
name|sq
condition|)
name|fputs
argument_list|(
literal|"'\\'"
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
name|ch
argument_list|)
expr_stmt|;
block|}
name|putchar
argument_list|(
name|sq
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
block|}
else|else
name|puts
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Output a revision, only if filter allows it */
end_comment
begin_function
DECL|function|show_rev
specifier|static
name|void
name|show_rev
parameter_list|(
name|int
name|type
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
if|if
condition|(
operator|!
operator|(
name|filter
operator|&
name|DO_REVS
operator|)
condition|)
return|return;
name|def
operator|=
name|NULL
expr_stmt|;
name|revs_count
operator|++
expr_stmt|;
if|if
condition|(
name|type
operator|!=
name|show_type
condition|)
name|putchar
argument_list|(
literal|'^'
argument_list|)
expr_stmt|;
if|if
condition|(
name|symbolic
operator|&&
name|name
condition|)
name|show
argument_list|(
name|name
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|abbrev
condition|)
name|show
argument_list|(
name|find_unique_abbrev
argument_list|(
name|sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|show
argument_list|(
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Output a flag, only if filter allows it. */
end_comment
begin_function
DECL|function|show_flag
specifier|static
name|int
name|show_flag
parameter_list|(
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
operator|!
operator|(
name|filter
operator|&
name|DO_FLAGS
operator|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|filter
operator|&
operator|(
name|is_rev_argument
argument_list|(
name|arg
argument_list|)
condition|?
name|DO_REVS
else|:
name|DO_NOREV
operator|)
condition|)
block|{
name|show
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_default
specifier|static
name|void
name|show_default
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|s
init|=
name|def
decl_stmt|;
if|if
condition|(
name|s
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|def
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|s
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|show_rev
argument_list|(
name|NORMAL
argument_list|,
name|sha1
argument_list|,
name|s
argument_list|)
expr_stmt|;
return|return;
block|}
block|}
block|}
end_function
begin_function
DECL|function|show_reference
specifier|static
name|int
name|show_reference
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|show_rev
argument_list|(
name|NORMAL
argument_list|,
name|sha1
argument_list|,
name|refname
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_datestring
specifier|static
name|void
name|show_datestring
parameter_list|(
specifier|const
name|char
modifier|*
name|flag
parameter_list|,
specifier|const
name|char
modifier|*
name|datestr
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|100
index|]
decl_stmt|;
comment|/* date handling requires both flags and revs */
if|if
condition|(
operator|(
name|filter
operator|&
operator|(
name|DO_FLAGS
operator||
name|DO_REVS
operator|)
operator|)
operator|!=
operator|(
name|DO_FLAGS
operator||
name|DO_REVS
operator|)
condition|)
return|return;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|"%s%lu"
argument_list|,
name|flag
argument_list|,
name|approxidate
argument_list|(
name|datestr
argument_list|)
argument_list|)
expr_stmt|;
name|show
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_file
specifier|static
name|int
name|show_file
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
name|show_default
argument_list|()
expr_stmt|;
if|if
condition|(
operator|(
name|filter
operator|&
operator|(
name|DO_NONFLAGS
operator||
name|DO_NOREV
operator|)
operator|)
operator|==
operator|(
name|DO_NONFLAGS
operator||
name|DO_NOREV
operator|)
condition|)
block|{
name|show
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
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
name|int
name|i
decl_stmt|,
name|as_is
init|=
literal|0
decl_stmt|,
name|verify
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|prefix
init|=
name|setup_git_directory
argument_list|()
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|dotdot
decl_stmt|;
if|if
condition|(
name|as_is
condition|)
block|{
name|show_file
argument_list|(
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-n"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|++
name|i
operator|>=
name|argc
condition|)
name|die
argument_list|(
literal|"-n requires an argument"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|filter
operator|&
name|DO_FLAGS
operator|)
operator|&&
operator|(
name|filter
operator|&
name|DO_REVS
operator|)
condition|)
block|{
name|show
argument_list|(
name|arg
argument_list|)
expr_stmt|;
name|show
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-n"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
if|if
condition|(
operator|(
name|filter
operator|&
name|DO_FLAGS
operator|)
operator|&&
operator|(
name|filter
operator|&
name|DO_REVS
operator|)
condition|)
name|show
argument_list|(
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|as_is
operator|=
literal|1
expr_stmt|;
comment|/* Pass on the "--" if we show anything but files.. */
if|if
condition|(
name|filter
operator|&
operator|(
name|DO_FLAGS
operator||
name|DO_REVS
operator|)
condition|)
name|show_file
argument_list|(
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--default"
argument_list|)
condition|)
block|{
name|def
operator|=
name|argv
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
name|i
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--revs-only"
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
name|DO_NOREV
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--no-revs"
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
name|DO_REVS
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--flags"
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
name|DO_NONFLAGS
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--no-flags"
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
name|DO_FLAGS
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--verify"
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
operator|(
name|DO_FLAGS
operator||
name|DO_NOREV
operator|)
expr_stmt|;
name|verify
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--short"
argument_list|)
operator|||
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--short="
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|filter
operator|&=
operator|~
operator|(
name|DO_FLAGS
operator||
name|DO_NOREV
operator|)
expr_stmt|;
name|verify
operator|=
literal|1
expr_stmt|;
name|abbrev
operator|=
name|DEFAULT_ABBREV
expr_stmt|;
if|if
condition|(
name|arg
index|[
literal|7
index|]
operator|==
literal|'='
condition|)
name|abbrev
operator|=
name|strtoul
argument_list|(
name|arg
operator|+
literal|8
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|abbrev
operator|<
name|MINIMUM_ABBREV
condition|)
name|abbrev
operator|=
name|MINIMUM_ABBREV
expr_stmt|;
elseif|else
if|if
condition|(
literal|40
operator|<=
name|abbrev
condition|)
name|abbrev
operator|=
literal|40
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--sq"
argument_list|)
condition|)
block|{
name|output_sq
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--not"
argument_list|)
condition|)
block|{
name|show_type
operator|^=
name|REVERSED
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--symbolic"
argument_list|)
condition|)
block|{
name|symbolic
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--all"
argument_list|)
condition|)
block|{
name|for_each_ref
argument_list|(
name|show_reference
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--show-prefix"
argument_list|)
condition|)
block|{
if|if
condition|(
name|prefix
condition|)
name|puts
argument_list|(
name|prefix
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--show-cdup"
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|pfx
init|=
name|prefix
decl_stmt|;
while|while
condition|(
name|pfx
condition|)
block|{
name|pfx
operator|=
name|strchr
argument_list|(
name|pfx
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
name|pfx
condition|)
block|{
name|pfx
operator|++
expr_stmt|;
name|printf
argument_list|(
literal|"../"
argument_list|)
expr_stmt|;
block|}
block|}
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--git-dir"
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|gitdir
init|=
name|getenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
decl_stmt|;
specifier|static
name|char
name|cwd
index|[
name|PATH_MAX
index|]
decl_stmt|;
if|if
condition|(
name|gitdir
condition|)
block|{
name|puts
argument_list|(
name|gitdir
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefix
condition|)
block|{
name|puts
argument_list|(
literal|".git"
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|cwd
argument_list|,
name|PATH_MAX
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to get current working directory"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s/.git\n"
argument_list|,
name|cwd
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--since="
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|show_datestring
argument_list|(
literal|"--max-age="
argument_list|,
name|arg
operator|+
literal|8
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--after="
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|show_datestring
argument_list|(
literal|"--max-age="
argument_list|,
name|arg
operator|+
literal|8
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--before="
argument_list|,
literal|9
argument_list|)
condition|)
block|{
name|show_datestring
argument_list|(
literal|"--min-age="
argument_list|,
name|arg
operator|+
literal|9
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--until="
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|show_datestring
argument_list|(
literal|"--min-age="
argument_list|,
name|arg
operator|+
literal|8
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|show_flag
argument_list|(
name|arg
argument_list|)
operator|&&
name|verify
condition|)
name|die
argument_list|(
literal|"Needed a single revision"
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* Not a flag argument */
name|dotdot
operator|=
name|strstr
argument_list|(
name|arg
argument_list|,
literal|".."
argument_list|)
expr_stmt|;
if|if
condition|(
name|dotdot
condition|)
block|{
name|unsigned
name|char
name|end
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|n
init|=
name|dotdot
operator|+
literal|2
decl_stmt|;
operator|*
name|dotdot
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
operator|*
name|n
condition|)
name|n
operator|=
literal|"HEAD"
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|n
argument_list|,
name|end
argument_list|)
condition|)
block|{
name|show_rev
argument_list|(
name|NORMAL
argument_list|,
name|end
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|show_rev
argument_list|(
name|REVERSED
argument_list|,
name|sha1
argument_list|,
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
block|}
operator|*
name|dotdot
operator|=
literal|'.'
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|show_rev
argument_list|(
name|NORMAL
argument_list|,
name|sha1
argument_list|,
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|*
name|arg
operator|==
literal|'^'
operator|&&
operator|!
name|get_sha1
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|show_rev
argument_list|(
name|REVERSED
argument_list|,
name|sha1
argument_list|,
name|arg
operator|+
literal|1
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|as_is
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|show_file
argument_list|(
name|arg
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|verify
condition|)
name|die
argument_list|(
literal|"Needed a single revision"
argument_list|)
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|arg
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"'%s': %s"
argument_list|,
name|arg
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|show_default
argument_list|()
expr_stmt|;
if|if
condition|(
name|verify
operator|&&
name|revs_count
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"Needed a single revision"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
