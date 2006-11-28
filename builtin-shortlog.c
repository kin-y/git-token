begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
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
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"path-list.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_decl_stmt
DECL|variable|shortlog_usage
specifier|static
specifier|const
name|char
name|shortlog_usage
index|[]
init|=
literal|"git-shortlog [-n] [-s] [<commit-id>... ]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|common_repo_prefix
specifier|static
name|char
modifier|*
name|common_repo_prefix
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|compare_by_number
specifier|static
name|int
name|compare_by_number
parameter_list|(
specifier|const
name|void
modifier|*
name|a1
parameter_list|,
specifier|const
name|void
modifier|*
name|a2
parameter_list|)
block|{
specifier|const
name|struct
name|path_list_item
modifier|*
name|i1
init|=
name|a1
decl_stmt|,
modifier|*
name|i2
init|=
name|a2
decl_stmt|;
specifier|const
name|struct
name|path_list
modifier|*
name|l1
init|=
name|i1
operator|->
name|util
decl_stmt|,
modifier|*
name|l2
init|=
name|i2
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|l1
operator|->
name|nr
operator|<
name|l2
operator|->
name|nr
condition|)
return|return
literal|1
return|;
elseif|else
if|if
condition|(
name|l1
operator|->
name|nr
operator|==
name|l2
operator|->
name|nr
condition|)
return|return
literal|0
return|;
else|else
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|mailmap
specifier|static
name|struct
name|path_list
name|mailmap
init|=
block|{
name|NULL
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_mailmap
specifier|static
name|int
name|read_mailmap
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|FILE
modifier|*
name|f
init|=
name|fopen
argument_list|(
name|filename
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
if|if
condition|(
name|f
operator|==
name|NULL
condition|)
return|return
literal|1
return|;
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|f
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|char
modifier|*
name|end_of_name
decl_stmt|,
modifier|*
name|left_bracket
decl_stmt|,
modifier|*
name|right_bracket
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|,
modifier|*
name|email
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|buffer
index|[
literal|0
index|]
operator|==
literal|'#'
condition|)
block|{
specifier|static
specifier|const
name|char
name|abbrev
index|[]
init|=
literal|"# repo-abbrev:"
decl_stmt|;
name|int
name|abblen
init|=
sizeof|sizeof
argument_list|(
name|abbrev
argument_list|)
operator|-
literal|1
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buffer
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|&&
name|buffer
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buffer
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|buffer
argument_list|,
name|abbrev
argument_list|,
name|abblen
argument_list|)
condition|)
block|{
name|char
modifier|*
name|cp
decl_stmt|;
if|if
condition|(
name|common_repo_prefix
condition|)
name|free
argument_list|(
name|common_repo_prefix
argument_list|)
expr_stmt|;
name|common_repo_prefix
operator|=
name|xmalloc
argument_list|(
name|len
argument_list|)
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|buffer
operator|+
name|abblen
init|;
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|;
name|cp
operator|++
control|)
empty_stmt|;
comment|/* nothing */
name|strcpy
argument_list|(
name|common_repo_prefix
argument_list|,
name|cp
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
if|if
condition|(
operator|(
name|left_bracket
operator|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|'<'
argument_list|)
operator|)
operator|==
name|NULL
condition|)
continue|continue;
if|if
condition|(
operator|(
name|right_bracket
operator|=
name|strchr
argument_list|(
name|left_bracket
operator|+
literal|1
argument_list|,
literal|'>'
argument_list|)
operator|)
operator|==
name|NULL
condition|)
continue|continue;
if|if
condition|(
name|right_bracket
operator|==
name|left_bracket
operator|+
literal|1
condition|)
continue|continue;
for|for
control|(
name|end_of_name
operator|=
name|left_bracket
init|;
name|end_of_name
operator|!=
name|buffer
operator|&&
name|isspace
argument_list|(
name|end_of_name
index|[
operator|-
literal|1
index|]
argument_list|)
condition|;
name|end_of_name
operator|--
control|)
comment|/* keep on looking */
if|if
condition|(
name|end_of_name
operator|==
name|buffer
condition|)
continue|continue;
name|name
operator|=
name|xmalloc
argument_list|(
name|end_of_name
operator|-
name|buffer
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strlcpy
argument_list|(
name|name
argument_list|,
name|buffer
argument_list|,
name|end_of_name
operator|-
name|buffer
operator|+
literal|1
argument_list|)
expr_stmt|;
name|email
operator|=
name|xmalloc
argument_list|(
name|right_bracket
operator|-
name|left_bracket
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|right_bracket
operator|-
name|left_bracket
operator|-
literal|1
condition|;
name|i
operator|++
control|)
name|email
index|[
name|i
index|]
operator|=
name|tolower
argument_list|(
name|left_bracket
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
expr_stmt|;
name|email
index|[
name|right_bracket
operator|-
name|left_bracket
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
name|path_list_insert
argument_list|(
name|email
argument_list|,
operator|&
name|mailmap
argument_list|)
operator|->
name|util
operator|=
name|name
expr_stmt|;
block|}
name|fclose
argument_list|(
name|f
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|map_email
specifier|static
name|int
name|map_email
parameter_list|(
name|char
modifier|*
name|email
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|int
name|maxlen
parameter_list|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|struct
name|path_list_item
modifier|*
name|item
decl_stmt|;
comment|/* autocomplete common developers */
name|p
operator|=
name|strchr
argument_list|(
name|email
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
return|return
literal|0
return|;
operator|*
name|p
operator|=
literal|'\0'
expr_stmt|;
comment|/* downcase the email address */
for|for
control|(
name|p
operator|=
name|email
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
operator|*
name|p
operator|=
name|tolower
argument_list|(
operator|*
name|p
argument_list|)
expr_stmt|;
name|item
operator|=
name|path_list_lookup
argument_list|(
name|email
argument_list|,
operator|&
name|mailmap
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|!=
name|NULL
condition|)
block|{
specifier|const
name|char
modifier|*
name|realname
init|=
operator|(
specifier|const
name|char
operator|*
operator|)
name|item
operator|->
name|util
decl_stmt|;
name|strncpy
argument_list|(
name|name
argument_list|,
name|realname
argument_list|,
name|maxlen
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
DECL|function|insert_author_oneline
specifier|static
name|void
name|insert_author_oneline
parameter_list|(
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
name|int
name|authorlen
parameter_list|,
specifier|const
name|char
modifier|*
name|oneline
parameter_list|,
name|int
name|onelinelen
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|dot3
init|=
name|common_repo_prefix
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|struct
name|path_list_item
modifier|*
name|item
decl_stmt|;
name|struct
name|path_list
modifier|*
name|onelines
decl_stmt|;
while|while
condition|(
name|authorlen
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
name|author
index|[
name|authorlen
operator|-
literal|1
index|]
argument_list|)
condition|)
name|authorlen
operator|--
expr_stmt|;
name|buffer
operator|=
name|xmalloc
argument_list|(
name|authorlen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
argument_list|,
name|author
argument_list|,
name|authorlen
argument_list|)
expr_stmt|;
name|buffer
index|[
name|authorlen
index|]
operator|=
literal|'\0'
expr_stmt|;
name|item
operator|=
name|path_list_insert
argument_list|(
name|buffer
argument_list|,
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|->
name|util
operator|==
name|NULL
condition|)
name|item
operator|->
name|util
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|path_list
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|oneline
argument_list|,
literal|"[PATCH"
argument_list|,
literal|6
argument_list|)
condition|)
block|{
name|char
modifier|*
name|eob
init|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|']'
argument_list|)
decl_stmt|;
if|if
condition|(
name|eob
condition|)
block|{
while|while
condition|(
name|isspace
argument_list|(
name|eob
index|[
literal|1
index|]
argument_list|)
operator|&&
name|eob
index|[
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|eob
operator|++
expr_stmt|;
if|if
condition|(
name|eob
operator|-
name|oneline
operator|<
name|onelinelen
condition|)
block|{
name|onelinelen
operator|-=
name|eob
operator|-
name|oneline
expr_stmt|;
name|oneline
operator|=
name|eob
expr_stmt|;
block|}
block|}
block|}
while|while
condition|(
name|onelinelen
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
name|oneline
index|[
literal|0
index|]
argument_list|)
condition|)
block|{
name|oneline
operator|++
expr_stmt|;
name|onelinelen
operator|--
expr_stmt|;
block|}
while|while
condition|(
name|onelinelen
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
name|oneline
index|[
name|onelinelen
operator|-
literal|1
index|]
argument_list|)
condition|)
name|onelinelen
operator|--
expr_stmt|;
name|buffer
operator|=
name|xmalloc
argument_list|(
name|onelinelen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
argument_list|,
name|oneline
argument_list|,
name|onelinelen
argument_list|)
expr_stmt|;
name|buffer
index|[
name|onelinelen
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|dot3
condition|)
block|{
name|int
name|dot3len
init|=
name|strlen
argument_list|(
name|dot3
argument_list|)
decl_stmt|;
if|if
condition|(
name|dot3len
operator|>
literal|5
condition|)
block|{
while|while
condition|(
operator|(
name|p
operator|=
name|strstr
argument_list|(
name|buffer
argument_list|,
name|dot3
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|taillen
init|=
name|strlen
argument_list|(
name|p
argument_list|)
operator|-
name|dot3len
decl_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
literal|"/.../"
argument_list|,
literal|5
argument_list|)
expr_stmt|;
name|memmove
argument_list|(
name|p
operator|+
literal|5
argument_list|,
name|p
operator|+
name|dot3len
argument_list|,
name|taillen
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|onelines
operator|=
name|item
operator|->
name|util
expr_stmt|;
if|if
condition|(
name|onelines
operator|->
name|nr
operator|>=
name|onelines
operator|->
name|alloc
condition|)
block|{
name|onelines
operator|->
name|alloc
operator|=
name|alloc_nr
argument_list|(
name|onelines
operator|->
name|nr
argument_list|)
expr_stmt|;
name|onelines
operator|->
name|items
operator|=
name|xrealloc
argument_list|(
name|onelines
operator|->
name|items
argument_list|,
name|onelines
operator|->
name|alloc
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|path_list_item
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|onelines
operator|->
name|items
index|[
name|onelines
operator|->
name|nr
index|]
operator|.
name|util
operator|=
name|NULL
expr_stmt|;
name|onelines
operator|->
name|items
index|[
name|onelines
operator|->
name|nr
operator|++
index|]
operator|.
name|path
operator|=
name|buffer
expr_stmt|;
block|}
end_function
begin_function
DECL|function|read_from_stdin
specifier|static
name|void
name|read_from_stdin
parameter_list|(
name|struct
name|path_list
modifier|*
name|list
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|char
modifier|*
name|bob
decl_stmt|;
if|if
condition|(
operator|(
name|buffer
index|[
literal|0
index|]
operator|==
literal|'A'
operator|||
name|buffer
index|[
literal|0
index|]
operator|==
literal|'a'
operator|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|buffer
operator|+
literal|1
argument_list|,
literal|"uthor: "
argument_list|,
literal|7
argument_list|)
operator|&&
operator|(
name|bob
operator|=
name|strchr
argument_list|(
name|buffer
operator|+
literal|7
argument_list|,
literal|'<'
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|char
name|buffer2
index|[
literal|1024
index|]
decl_stmt|,
name|offset
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|map_email
argument_list|(
name|bob
operator|+
literal|1
argument_list|,
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|)
condition|)
name|bob
operator|=
name|buffer
operator|+
name|strlen
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
else|else
block|{
name|offset
operator|=
literal|8
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
name|bob
index|[
operator|-
literal|1
index|]
argument_list|)
condition|)
name|bob
operator|--
expr_stmt|;
block|}
while|while
condition|(
name|fgets
argument_list|(
name|buffer2
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer2
argument_list|)
argument_list|,
name|stdin
argument_list|)
operator|&&
name|buffer2
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
empty_stmt|;
comment|/* chomp input */
if|if
condition|(
name|fgets
argument_list|(
name|buffer2
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer2
argument_list|)
argument_list|,
name|stdin
argument_list|)
condition|)
name|insert_author_oneline
argument_list|(
name|list
argument_list|,
name|buffer
operator|+
name|offset
argument_list|,
name|bob
operator|-
name|buffer
operator|-
name|offset
argument_list|,
name|buffer2
argument_list|,
name|strlen
argument_list|(
name|buffer2
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|get_from_rev
specifier|static
name|void
name|get_from_rev
parameter_list|(
name|struct
name|rev_info
modifier|*
name|rev
parameter_list|,
name|struct
name|path_list
modifier|*
name|list
parameter_list|)
block|{
name|char
name|scratch
index|[
literal|1024
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|prepare_revision_walk
argument_list|(
name|rev
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
name|rev
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|char
modifier|*
name|author
init|=
name|NULL
decl_stmt|,
modifier|*
name|oneline
decl_stmt|,
modifier|*
name|buffer
decl_stmt|;
name|int
name|authorlen
init|=
name|authorlen
decl_stmt|,
name|onelinelen
decl_stmt|;
comment|/* get author and oneline */
for|for
control|(
name|buffer
operator|=
name|commit
operator|->
name|buffer
init|;
name|buffer
operator|&&
operator|*
name|buffer
operator|!=
literal|'\0'
operator|&&
operator|*
name|buffer
operator|!=
literal|'\n'
condition|;
control|)
block|{
name|char
modifier|*
name|eol
init|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|'\n'
argument_list|)
decl_stmt|;
if|if
condition|(
name|eol
operator|==
name|NULL
condition|)
name|eol
operator|=
name|buffer
operator|+
name|strlen
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
else|else
name|eol
operator|++
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|buffer
argument_list|,
literal|"author "
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|char
modifier|*
name|bracket
init|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|'<'
argument_list|)
decl_stmt|;
if|if
condition|(
name|bracket
operator|==
name|NULL
operator|||
name|bracket
operator|>
name|eol
condition|)
name|die
argument_list|(
literal|"Invalid commit buffer: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|map_email
argument_list|(
name|bracket
operator|+
literal|1
argument_list|,
name|scratch
argument_list|,
sizeof|sizeof
argument_list|(
name|scratch
argument_list|)
argument_list|)
condition|)
block|{
name|author
operator|=
name|scratch
expr_stmt|;
name|authorlen
operator|=
name|strlen
argument_list|(
name|scratch
argument_list|)
expr_stmt|;
block|}
else|else
block|{
while|while
condition|(
name|bracket
index|[
operator|-
literal|1
index|]
operator|==
literal|' '
condition|)
name|bracket
operator|--
expr_stmt|;
name|author
operator|=
name|buffer
operator|+
literal|7
expr_stmt|;
name|authorlen
operator|=
name|bracket
operator|-
name|buffer
operator|-
literal|7
expr_stmt|;
block|}
block|}
name|buffer
operator|=
name|eol
expr_stmt|;
block|}
if|if
condition|(
name|author
operator|==
name|NULL
condition|)
name|die
argument_list|(
literal|"Missing author: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
operator|==
name|NULL
operator|||
operator|*
name|buffer
operator|==
literal|'\0'
condition|)
block|{
name|oneline
operator|=
literal|"<none>"
expr_stmt|;
name|onelinelen
operator|=
sizeof|sizeof
argument_list|(
name|oneline
argument_list|)
operator|+
literal|1
expr_stmt|;
block|}
else|else
block|{
name|char
modifier|*
name|eol
decl_stmt|;
name|oneline
operator|=
name|buffer
operator|+
literal|1
expr_stmt|;
name|eol
operator|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|eol
operator|==
name|NULL
condition|)
name|onelinelen
operator|=
name|strlen
argument_list|(
name|oneline
argument_list|)
expr_stmt|;
else|else
name|onelinelen
operator|=
name|eol
operator|-
name|oneline
expr_stmt|;
block|}
name|insert_author_oneline
argument_list|(
name|list
argument_list|,
name|author
argument_list|,
name|authorlen
argument_list|,
name|oneline
argument_list|,
name|onelinelen
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_shortlog
name|int
name|cmd_shortlog
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|struct
name|path_list
name|list
init|=
block|{
name|NULL
block|,
literal|0
block|,
literal|0
block|,
literal|1
block|}
decl_stmt|;
name|int
name|i
decl_stmt|,
name|j
decl_stmt|,
name|sort_by_number
init|=
literal|0
decl_stmt|,
name|summary
init|=
literal|0
decl_stmt|;
comment|/* since -n is a shadowed rev argument, parse our args first */
while|while
condition|(
name|argc
operator|>
literal|1
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-n"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--numbered"
argument_list|)
condition|)
name|sort_by_number
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-s"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--summary"
argument_list|)
condition|)
name|summary
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--help"
argument_list|)
condition|)
name|usage
argument_list|(
name|shortlog_usage
argument_list|)
expr_stmt|;
else|else
break|break;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|die
argument_list|(
literal|"unrecognized argument: %s"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|access
argument_list|(
literal|".mailmap"
argument_list|,
name|R_OK
argument_list|)
condition|)
name|read_mailmap
argument_list|(
literal|".mailmap"
argument_list|)
expr_stmt|;
if|if
condition|(
name|rev
operator|.
name|pending
operator|.
name|nr
operator|==
literal|0
condition|)
name|read_from_stdin
argument_list|(
operator|&
name|list
argument_list|)
expr_stmt|;
else|else
name|get_from_rev
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|sort_by_number
condition|)
name|qsort
argument_list|(
name|list
operator|.
name|items
argument_list|,
name|list
operator|.
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|path_list_item
argument_list|)
argument_list|,
name|compare_by_number
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|path_list
modifier|*
name|onelines
init|=
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|util
decl_stmt|;
if|if
condition|(
name|summary
condition|)
block|{
name|printf
argument_list|(
literal|"%s: %d\n"
argument_list|,
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|path
argument_list|,
name|onelines
operator|->
name|nr
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf
argument_list|(
literal|"%s (%d):\n"
argument_list|,
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|path
argument_list|,
name|onelines
operator|->
name|nr
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
name|onelines
operator|->
name|nr
operator|-
literal|1
init|;
name|j
operator|>=
literal|0
condition|;
name|j
operator|--
control|)
name|printf
argument_list|(
literal|"      %s\n"
argument_list|,
name|onelines
operator|->
name|items
index|[
name|j
index|]
operator|.
name|path
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
block|}
name|onelines
operator|->
name|strdup_paths
operator|=
literal|1
expr_stmt|;
name|path_list_clear
argument_list|(
name|onelines
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|onelines
argument_list|)
expr_stmt|;
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|util
operator|=
name|NULL
expr_stmt|;
block|}
name|list
operator|.
name|strdup_paths
operator|=
literal|1
expr_stmt|;
name|path_list_clear
argument_list|(
operator|&
name|list
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|mailmap
operator|.
name|strdup_paths
operator|=
literal|1
expr_stmt|;
name|path_list_clear
argument_list|(
operator|&
name|mailmap
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
